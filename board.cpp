#include "board.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

Board::Tile Board::NULL_TILE = Tile();

static const int offsetX_[4] { +1, +1, -1, -1 };
static const int offsetY_[4] { -1, +1, +1, -1 };
static std::map<int, std::string> algebraicNotaionFileMap
{
    { 0, "a"},
    { 1, "b"},
    { 2, "c"},
    { 3, "d"},
    { 4, "e"},
    { 5, "f"},
    { 6, "g"},
    { 7, "h"}
};

Board::Board()
{

    Reset();
    _moveLog.clear();
}

void Board::Reset()
{
    clearBoard();
    for(int y = 0; y < NUM_CHECKER_ROW_FOR_ONE_SIDE; ++y)
    {
        for(int x = (y + 1) % 2; x < BOARD_SIZE; x += 2)
        {
            setPiece(x,y,Alliance::BLUE);
        }
    }

    for(int y = BOARD_SIZE - NUM_CHECKER_ROW_FOR_ONE_SIDE; y < BOARD_SIZE; ++y)
    {
        for(int x = (y + 1) % 2; x < BOARD_SIZE; x += 2)
        {
            setPiece(x,y,Alliance::RED);
        }
    }

    count23 = 0; // 2-3 piece ending limit 5 moves
    count45 = 0; // 4-5 piece ending limit 30 moves
    count67 = 0; // 6-7 piece ending limit 60 moves
    count15 = 0;
    _mapRep.clear();
    _moveLog.clear();
    _hash = CalcHash();
}

void Board::clearBoard()
{
    for(int y = 0; y < BOARD_SIZE; ++y)
    {
        for(int x = 0; x < BOARD_SIZE; ++x)
        {
            _grid[y][x] = Tile(x, y);
        }
    }
}

bool Board::checkCrown(const Board::Piece &piece) const
{
    return (piece.alliance == Alliance::RED && piece.y == 0)
            || (piece.alliance == Alliance::BLUE && piece.y == BOARD_SIZE - 1);
}

void Board::CalcPieceCount(int &count_red_pieces, int &count_red_kings, int &count_blue_pieces, int &count_blue_kings) const
{
    count_red_pieces = 0;
    count_red_kings = 0;
    count_blue_pieces = 0;
    count_blue_kings = 0;

    for(int y = 0; y < BOARD_SIZE; ++y)
    {
        for(int x = 0; x < BOARD_SIZE; ++x)
        {
            if(_grid[y][x].hasPiece())
            {
                Alliance alliance = _grid[y][x].piece.alliance;
                bool isKing = _grid[y][x].piece.isKing;
                if(alliance == Alliance::RED)
                {
                    if(isKing)
                        count_red_kings++;
                    else
                        count_red_pieces++;
                }
                else if(alliance == Alliance::BLUE)
                {
                    if(isKing)
                        count_blue_kings++;
                    else
                        count_blue_pieces++;
                }
            }
         }
    }
}


std::string Board::CalcHash() const
{
    std::stringstream ss;
    for(int y = 0; y < BOARD_SIZE; ++y)
    {
        for(int x = 0; x < BOARD_SIZE; ++x)
        {
            if(_grid[y][x].hasPiece())
            {
                Alliance alliance = _grid[y][x].piece.alliance;
                bool isKing = _grid[y][x].piece.isKing;
                if(alliance == Alliance::RED)
                {
                    ss << (isKing ? 'R' : 'r');
                }
                else if(alliance == Alliance::BLUE)
                {
                    ss << (isKing ? 'B' : 'b');
                }
            }
            else
            {
                ss << (_grid[y][x].isDark() ? '.' : ' ');
            }
        }
    }
    return ss.str();
}

int Board::GetTotalPieces() const
{
    int count_red_pieces, count_red_kings, count_blue_pieces, count_blue_kings;
    CalcPieceCount(count_red_pieces, count_red_kings, count_blue_pieces, count_blue_kings);
    return count_red_pieces + count_red_kings + count_blue_pieces + count_blue_kings;
}

bool Board::isValidTile(int x, int y) const
{
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

bool Board::isTileEmpty(int x, int y) const
{
    return isValidTile(x,y) && _grid[y][x].isEmpty();
}

Board::Tile Board::getTile(int x, int y) const
{
    return isValidTile(x, y) ? _grid[y][x] : NULL_TILE;
}

bool Board::setPiece(int x, int y, Board::Alliance alliance, bool isKing)
{
    if(isValidTile(x,y))
    {
        _grid[y][x].piece = Piece(x,y,alliance, isKing);
        return true;
    }
    return false;
}

bool Board::makeMove(Move move)
{
    if(move.empty()) return false;
    int startX = move[0].start.x;
    int startY = move[0].start.y;
    Piece movedPiece = move[0].start.piece;
    int endX = move[move.size() - 1].end.x;
    int endY = move[move.size() - 1].end.y;

    if(!(isValidTile(startX, startY) && _grid[startY][startX].isDark()
            && isValidTile(endX, endY) && _grid[endY][endX].isDark()))
        return false;

    _grid[startY][startX].removePiece();
    _grid[endY][endX].setPiece(movedPiece);

    bool coronation = false;
    if(move.size() > 1 || (move.size() == 1 && move.at(0).isJump()))
    {
        for(auto it = move.begin(); it != move.end(); ++it)
        {
            int cx = it->captured.x;
            int cy = it->captured.y;
            _grid[cy][cx].removePiece();
            int ey = it->end.y;
            if((movedPiece.alliance == Alliance::RED && ey == 0)
                    || (movedPiece.alliance == Alliance::BLUE && ey == BOARD_SIZE - 1))
            {
                coronation = true;
                it->IsCoronation = true;
            }
        }
    }
    if((move.size() == 1 && !move.at(0).isJump()))
    {
        if((movedPiece.alliance == Alliance::RED && endY == 0)
                || (movedPiece.alliance == Alliance::BLUE && endY == BOARD_SIZE - 1))
        {
            coronation = true;
            move[0].IsCoronation = true;
        }
    }

    if(coronation)
    {
        _grid[endY][endX].piece.crown();
    }
    _moveLog.push_back(move);

    _hash = CalcHash();
    _mapRep[_hash]++;
    /////////////////////////////////////////////////////////////////////
    int count_red_pieces = 0, count_red_kings = 0, count_blue_pieces = 0, count_blue_kings = 0;
    CalcPieceCount(count_red_pieces, count_red_kings, count_blue_pieces, count_blue_kings);

    if((count_red_pieces == 0 && count_red_kings >= 3 && count_blue_kings == 1)
            || (count_blue_pieces == 0 && count_blue_kings >= 3 && count_blue_pieces == 0))
    {
        ++count15;
    }

    int reds_total = count_red_pieces + count_red_kings;
    int blues_total = count_blue_pieces + count_blue_kings;
    int total = reds_total + blues_total;

    int balance = (count_red_pieces + 3 * count_red_kings) - (count_blue_pieces + 3 * count_blue_kings);
    if(total == 2 || total == 3)
    {
        count45 = 0;
        count67 = 0;
        if(balance != oldBalance)
        {
            oldBalance = balance;
            count23 = 0;
        }
        else
        {
            ++count23;
        }
    }
    else if(total == 4 || total == 6)
    {
        if(balance != oldBalance)
        {
            oldBalance = balance;
            count45 = 0;
        }
        else
        {
            ++count45;
        }
    }
    else if(total == 6 || total == 7)
    {
        count67 = 0;
        if(balance != oldBalance)
        {
            oldBalance = balance;
            count67 = 0;
        }
        else
        {
            ++count67;
        }
    }

    return true;
}

GameStatus Board::GetGameStatus() const
{
    if(!_mapRep.empty() && _mapRep.at(_hash) >= 3)
    {
        //std::cout << "Draw by threefold repetition!\n";
        return GameStatus::DRAW;
    }

    if(count15 >= 2 * 15)
    {
        //std::cout << "No king capture within 15 moves!\n";
        return GameStatus::DRAW;
    }

    if(count23 >= 2 * 5)
    {
        //std::cout << "No captures or coronations in the last 5 moves for 2-3 piece ending!\n";
        return GameStatus::DRAW;
    }

    if(count45 >= 2 * 30)
    {
        //std::cout << "No captures or coronations in the last 30 moves for 4-5 piece ending!\n";
        return GameStatus::DRAW;
    }

    if(count67 >= 2 * 60)
    {
        //std::cout << "No captures or coronations in the last 60 moves for 6-7 piece ending!\n";
        return GameStatus::DRAW;
    }

    if(_moveLog.size() > 30)
    {
        int count = 0;
        for(auto it = _moveLog.rbegin(); it != _moveLog.rend(); ++it)
        {
            const Move& move = *it;
            for(const auto& step: move)
            {
                if(step.end.piece.isKing)
                    count++;
            }
        }

        if(count >= 30)
            return GameStatus::DRAW;
    }

    std::vector<Board::Move> redLegalMoves;
    calcLegalMoves(Alliance::RED, redLegalMoves);
    std::vector<Board::Move> blueLegalMoves;
    calcLegalMoves(Alliance::BLUE, blueLegalMoves);
    if(redLegalMoves.empty())
    {
        return GameStatus::BLUE_WON;
    }

    if(blueLegalMoves.empty())
    {
        return GameStatus::RED_WON;
    }

    return GameStatus::PLAY;
}

bool Board::isEndGameScenario() const
{
    return GetGameStatus() != GameStatus::PLAY;
}

void Board::undoLastMove()
{
    if(_moveLog.empty()) return;

    _mapRep[_hash]--;

    Move move = _moveLog[_moveLog.size() - 1];
    int startX = move[0].start.x;
    int startY = move[0].start.y;
    Piece movedPiece = move[0].start.piece;
    int endX = move[move.size() - 1].end.x;
    int endY = move[move.size() - 1].end.y;
    _grid[startY][startX].setPiece(movedPiece);
    _grid[endY][endX].removePiece();

    bool undoCoronation = false;
    if(move.size() > 1 || (move.size() == 1 && move.at(0).captured.x != -1))
    {
        for(auto it = move.begin(); it != move.end(); ++it)
        {
            int cx = it->captured.x;
            int cy = it->captured.y;
            _grid[cy][cx].setPiece(it->captured);
            undoCoronation |= it->IsCoronation;
        }
    }

    if(undoCoronation)
        _grid[startY][startX].piece.isKing = false;

    _moveLog.pop_back();
    _moveLog.shrink_to_fit();
    _hash = CalcHash();
}

void Board::calcLegalMoves(Alliance alliance, std::vector<Move> &moves) const
{
    moves.clear();
    for(int y = 0; y < BOARD_SIZE; ++y)
    {
        for(int x = 0; x < BOARD_SIZE; ++x)
        {
            if(getTile(x,y).hasPiece() && getTile(x,y).piece.alliance == alliance)
            {
                Move move;
                calcAllJumps(getTile(x,y).piece, move, moves);
            }
        }
    }

    if(!moves.empty()) return;

    for(int y = 0; y < BOARD_SIZE; ++y)
    {
        for(int x = 0; x < BOARD_SIZE; ++x)
        {
            Tile currTile = getTile(x, y);
            if(currTile.isEmpty())
                continue;
            if(currTile.piece.alliance != alliance)
                continue;
            if(currTile.piece.isKing)
            {
                for(int dir = 0; dir < 4; ++dir)
                {
                    for(int n = 1; n < BOARD_SIZE; ++n)
                    {
                        int nx = x + n * offsetX_[dir];
                        int ny = y + n * offsetY_[dir];
                        if(!isValidTile(nx, ny) || !isTileEmpty(nx, ny))
                            break;
                        Move move;
                        move.emplace_back(currTile, getTile(nx, ny));
                        moves.push_back(move);
                    }
                }
            }
            else
            {
                int nx, ny;
                int dy = alliance == Alliance::RED ? -1 : +1;
                for(int dx = -1; dx <= +1; dx += 2)
                {
                    nx = x + dx;
                    ny = y + dy;
                    if(isValidTile(nx, ny) && isTileEmpty(nx, ny))
                    {
                        Move move;
                        move.emplace_back(currTile, getTile(nx, ny));
                        moves.push_back(move);
                    }
                }
            }
        }
    }
}

void Board::calcAllJumps(Piece piece, Move move, std::vector<Move> &legalMoves) const
{
    Tile startTile = _grid[piece.y][piece.x];
    int N = piece.isKing ? BOARD_SIZE - 1 : 2;

    for(int dir = 0; dir < 4; ++dir)
    {
        bool targetDetected = false;
        Tile current = getTile(piece.x, piece.y);
        Piece target;

        for (int n = 1; n <= N; ++n)
        {
            int dx = n * offsetX_[dir];
            int dy = n * offsetY_[dir];
            int nx = piece.x + dx;
            int ny = piece.y + dy;

            if(!isValidTile(nx, ny))
                break;

            current = getTile(nx, ny);

            if(targetDetected)
            {
                if(current.hasPiece())
                    break;
                bool isSameTarget = false;
                for(auto it = move.begin(); it != move.end(); ++it)
                {
                    if(it->captured.x == target.x && it->captured.y == target.y)
                    {
                        isSameTarget = true;
                        break;
                    }
                }
                if(!isSameTarget)
                {
                    Step step = Step(startTile, current, target);
                    Move oldMove = move;
                    move.push_back(step);
                    Piece p = Piece(current.x, current.y, piece.alliance, piece.isKing);
                    if(checkCrown(p))
                    {
                        p.crown();
                    }

                    legalMoves.push_back(move);
                    calcAllJumps(p, move, legalMoves);
                    move = oldMove;
                }
            }
            else
            {
                if(current.hasPiece())
                {
                    if(current.piece.alliance != piece.alliance)
                    {
                        targetDetected = true;
                        target = current.piece;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }
}

int Board::score() const
{
    int score = 0;
    std::vector<Move> redLegalMoves;
    calcLegalMoves(Alliance::RED, redLegalMoves);
    std::vector<Move> blueLegalMoves;
    calcLegalMoves(Alliance::BLUE, blueLegalMoves);

    const int INF = 10000000;
    if(redLegalMoves.empty())
        return -INF;
    if(blueLegalMoves.empty())
        return INF;
    score += 10*redLegalMoves.size();
    score -= 10*blueLegalMoves.size();

    int num_red_kings = 0;
    int num_blue_kings = 0;
    for(int y = 0; y < BOARD_SIZE; ++y)
    {
        for(int x = 0; x < BOARD_SIZE; ++x)
        {
            Tile currTile = getTile(x,y);
            if(currTile.hasPiece())
            {
                if(currTile.piece.alliance == Alliance::RED)
                {
                    if(currTile.piece.isKing)
                        num_red_kings++;
                    score += currTile.piece.value;
                    score += (BOARD_SIZE - y);
                    if(x == 0 || x == BOARD_SIZE)
                        score -= currTile.piece.value / 2;

                    //retrasadas
                    if(x == 0 && y == BOARD_SIZE - 1)
                        score -= currTile.piece.value / 2;
                    else if(x == BOARD_SIZE -1 && y == BOARD_SIZE - 2)
                        score -= currTile.piece.value / 2;

                    if(y == BOARD_SIZE - 1)
                    {
                        //golden piece
                        if(x == BOARD_SIZE / 2)
                            score += 3 * currTile.piece.value;
                        else
                            score += currTile.piece.value / 2;
                    }

                }
                else if(currTile.piece.alliance == Alliance::BLUE)
                {
                    if(currTile.piece.isKing)
                        num_red_kings--;
                    score -= currTile.piece.value;
                    score -= (y + 1);
                    if(x == 0 || x == BOARD_SIZE)
                        score += currTile.piece.value / 2;

                    //retrasadas
                    if(x == BOARD_SIZE - 1 && y == 0)
                        score += currTile.piece.value / 2;
                    else if(x == 0 && y == 1)
                        score += currTile.piece.value / 2;

                    if(y == 0)
                    {
                        //golden piece
                        if(x == BOARD_SIZE / 2)
                            score -= 3 * currTile.piece.value;
                        else
                            score -= currTile.piece.value / 2;
                    }
                }
            }
        }
    }

    score += 10 * (num_red_kings - num_blue_kings);
    return score;
}

std::string Board::toString()
{
    std::stringstream ss;
    for(int y = 0; y < BOARD_SIZE; ++y)
    {
        std::string line;
        for(int x = 0; x < BOARD_SIZE; ++x)
        {
            if(_grid[y][x].hasPiece())
            {
                Piece p = _grid[y][x].piece;
                if(p.alliance == Alliance::RED)
                {
                    line += p.isKing ? "[R]" : "[r]";
                }
                else if(p.alliance == Alliance::BLUE)
                {
                    line += p.isKing ? "[B]" : "[b]";
                }
            }
            else
            {
                line += _grid[y][x].isDark() ?  "[.]" : "[ ]";
            }
        }
        line += "\n";
        ss << line;
    }
    return ss.str();
}

std::string Board::tileToAlgebraicNotation(const Board::Tile &tile)
{
    
    int x = tile.x;
    int y = tile.y;
    return algebraicNotaionFileMap[x] + std::to_string(BOARD_SIZE - y);
}

std::string Board::moveToAlgebraicNotation(const Board::Move &move)
{
    if(move.empty()) return "";
    std::string notation = tileToAlgebraicNotation(move.at(0).start);
    if(move.at(0).isJump())
    {
        for(auto &step: move)
        {
            notation += ":" + tileToAlgebraicNotation(step.end);
        }
    }
    else
    {
        notation +=  "-" + tileToAlgebraicNotation(move.at(0).end);
    }
    return notation;
}

const std::vector<Board::Move> &Board::getMoveLog() const
{
    return _moveLog;
}

