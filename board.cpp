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

    setupInitialPosition();
    moveLog_.clear();
}

void Board::setupInitialPosition()
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
}

void Board::clearBoard()
{
    for(int y = 0; y < BOARD_SIZE; ++y)
    {
        for(int x = 0; x < BOARD_SIZE; ++x)
        {
            grid_[y][x] = Tile(x, y);
        }
    }
}

bool Board::checkCrown(const Board::Piece &piece) const
{
    return (piece.alliance == Alliance::RED && piece.y == 0)
            || (piece.alliance == Alliance::BLUE && piece.y == BOARD_SIZE - 1);
}

bool Board::isEndGameScenario() const
{
    std::vector<Board::Move> redLegalMoves;
    calcLegalMoves(Alliance::RED, redLegalMoves);
    std::vector<Board::Move> blueLegalMoves;
    calcLegalMoves(Alliance::BLUE, blueLegalMoves);
    return redLegalMoves.empty() || blueLegalMoves.empty();
}

bool Board::isValidTile(int x, int y) const
{
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

bool Board::isTileEmpty(int x, int y) const
{
    return isValidTile(x,y) && grid_[y][x].isEmpty();
}

Board::Tile Board::getTile(int x, int y) const
{
    return isValidTile(x, y) ? grid_[y][x] : NULL_TILE;
}

bool Board::setPiece(int x, int y, Board::Alliance alliance, bool isKing)
{
    if(isValidTile(x,y))
    {
        grid_[y][x].piece = Piece(x,y,alliance, isKing);
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

    if(!(isValidTile(startX, startY) && grid_[startY][startX].isDark()
            && isValidTile(endX, endY) && grid_[endY][endX].isDark()))
        return false;

    grid_[startY][startX].removePiece();
    grid_[endY][endX].setPiece(movedPiece);

    bool coronation = false;
    if(move.size() > 1 || (move.size() == 1 && move.at(0).isJump()))
    {
        for(auto it = move.begin(); it != move.end(); ++it)
        {
            int cx = it->captured.x;
            int cy = it->captured.y;
            grid_[cy][cx].removePiece();
            int ey = it->end.y;
            if((movedPiece.alliance == Alliance::RED && ey == 0)
                    || (movedPiece.alliance == Alliance::BLUE && ey == BOARD_SIZE - 1))
            {
                coronation = true;
            }
        }
    }
    if((move.size() == 1 && !move.at(0).isJump()))
    {
        if((movedPiece.alliance == Alliance::RED && endY == 0)
                || (movedPiece.alliance == Alliance::BLUE && endY == BOARD_SIZE - 1))
        {
            coronation = true;
        }
    }

    if(coronation)
    {
        grid_[endY][endX].piece.crown();
    }
    moveLog_.push_back(move);
    return true;
}

void Board::undoLastMove()
{
    if(moveLog_.empty()) return;
    Move move = moveLog_[moveLog_.size() - 1];
    int startX = move[0].start.x;
    int startY = move[0].start.y;
    Piece movedPiece = move[0].start.piece;
    int endX = move[move.size() - 1].end.x;
    int endY = move[move.size() - 1].end.y;
    grid_[startY][startX].setPiece(movedPiece);
    grid_[endY][endX].removePiece();

    if(move.size() > 1 || (move.size() == 1 && move.at(0).captured.x != -1))
    {
        for(auto it = move.begin(); it != move.end(); ++it)
        {
            int cx = it->captured.x;
            int cy = it->captured.y;
            grid_[cy][cx].setPiece(it->captured);
        }
    }

    moveLog_.pop_back();
    moveLog_.shrink_to_fit();
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
    Tile startTile = grid_[piece.y][piece.x];
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
                    Piece p = Piece(current.x, current.y, piece.alliance, false);
                    if(checkCrown(p) /*|| piece.isKing*/)
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

    /*if(redLegalMoves.empty())
        return -10000;
    if(blueLegalMoves.empty())
        return 10000;*/
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

                    //golden piece
                    if(x == BOARD_SIZE / 2 && y == BOARD_SIZE - 1)
                        score += 3 * currTile.piece.value;
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

                    //golden piece
                    if(x == BOARD_SIZE / 2 - 1 && y == 0)
                        score -= 3 * currTile.piece.value;
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
            if(grid_[y][x].hasPiece())
            {
                Piece p = grid_[y][x].piece;
                if(p.alliance == Alliance::RED)
                {
                    line += p.isKing ? "R" : "r";
                }
                else if(p.alliance == Alliance::BLUE)
                {
                    line += p.isKing ? "B" : "b";
                }
            }
            else
            {
                line += "_";
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
            notation += " : " + tileToAlgebraicNotation(step.end);
        }
    }
    else
    {
        notation +=  " - " + tileToAlgebraicNotation(move.at(0).end);
    }
    return notation;
}

const std::vector<Board::Move> &Board::getMoveLog() const
{
    return moveLog_;
}
