#include "board.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

Board::Board()
{
    //clearBoard();
    setupInitialPosition();
    /*setPiece(0,7,Alliance::RED, true);
    setPiece(1,6,Alliance::BLUE, false);
    setPiece(3,4,Alliance::BLUE, false);
    setPiece(5,4,Alliance::BLUE, false);
    setPiece(5,2,Alliance::BLUE, false);
    setPiece(5,6,Alliance::BLUE, false);
    setPiece(3,6,Alliance::BLUE, false);
    setPiece(1,4,Alliance::BLUE, false);*/
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

bool Board::isValidTile(int x, int y) const
{
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

bool Board::isTileEmpty(int x, int y) const
{
    return getTile(x, y).isEmpty();
}

Board::Tile Board::getTile(int x, int y) const
{
    return isValidTile(x, y) ? grid_[y][x] : Tile();
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

    if(move.size() > 1 || (move.size() == 1 && move.at(0).captured.x != -1))
    {
        for(auto it = move.begin(); it != move.end(); ++it)
        {
            int cx = it->captured.x;
            int cy = it->captured.y;
            grid_[cy][cx].removePiece();
        }
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
}

void Board::calcLegalMoves(Alliance alliance, std::vector<Move> &moves) const
{
    moves.clear();
    for(int y = 0; y < BOARD_SIZE; ++y)
    {
        for(int x = 0; x < BOARD_SIZE; ++x)
        {
            if(!getTile(x,y).isEmpty())
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
            current = getTile(piece.x + n * offsetX_[dir], piece.y + n * offsetY_[dir]);
            if(!isValidTile(current.x, current.y))
                break;
            if(targetDetected)
            {
                if(!current.isEmpty())
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
                    if(checkCrown(p) || piece.isKing)
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
                if(current.hasPiece() && current.piece.alliance != piece.alliance)
                {
                    targetDetected = true;
                    target = current.piece;
                }
            }
        }
    }
}

int Board::score() const
{
    int score = 0;
    for(int y = 0; y < BOARD_SIZE; ++y)
    {
        for(int x = 0; x < BOARD_SIZE; ++x)
        {
            Tile currTile = getTile(x,y);
            if(currTile.hasPiece())
            {
                if(currTile.piece.alliance == Alliance::RED)
                {
                    score += currTile.piece.value;
                }
                else if(currTile.piece.alliance == Alliance::RED)
                {
                    score -= currTile.piece.value;
                }
            }
        }
    }
    return score;
}
