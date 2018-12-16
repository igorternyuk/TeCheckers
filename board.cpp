#include "board.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

Board::Board()
{
    setupInitialPosition();
    //getTile(2,7).setPiece(Piece(2,7,Alliance::RED));
}

void Board::setupInitialPosition()
{
    clearBoard();
    for(int y = 0; y < NUM_CHECKER_ROW_FOR_ONE_SIDE; ++y)
    {
        for(int x = (y + 1) % 2; x < BOARD_SIZE; x += 2)
        {
            grid_[y][x].piece = Piece(x,y,Alliance::BLUE);
        }
    }

    for(int y = BOARD_SIZE - NUM_CHECKER_ROW_FOR_ONE_SIDE; y < BOARD_SIZE; ++y)
    {
        for(int x = (y + 1) % 2; x < BOARD_SIZE; x += 2)
        {
            grid_[y][x].piece = Piece(x,y,Alliance::RED);
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

bool Board::makeMove(Move move)
{
    return false;
}

void Board::calcLegalMoves(Alliance alliance, std::vector<Move> &moves) const
{
    for(int y = 0; y < BOARD_SIZE; ++y)
    {
        for(int x = 0; x < BOARD_SIZE; ++x)
        {
            Tile currTile = getTile(x, y);
            if(currTile.isEmpty())
                continue;
            if(currTile.piece.isKing)
            {
                for(int n = 1; n < BOARD_SIZE; ++n)
                {
                    for(int dir = 0; dir < 4; ++dir)
                    {
                        int nx = x + n * offsetX_[dir];
                        int ny = y + n * offsetY_[dir];
                        if(isValidTile(nx, ny) && isTileEmpty(nx, ny))
                        {
                            Move move;
                            move.emplace_back(currTile, getTile(nx, ny));
                            moves.push_back(move);
                        }
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
    Tile startTile = getTile(piece.x, piece.y);
    if(startTile.isEmpty()) return;
    if(startTile.piece.isKing)
    {
        for(int dir = 0; dir < 4; ++dir)
        {
            bool enemyDetected = false;
            Tile current = getTile(startTile.x + offsetX_[dir], startTile.y + offsetY_[dir]);
            Piece captured;

            while (isValidTile(current.x, current.y)
                   && (current.hasPiece() && current.piece.alliance == piece.alliance))
            {
                if(enemyDetected)
                {
                    if(current.isEmpty())
                    {
                       bool isSameSpot = false;
                       for(auto it = move.begin(); it != move.end(); ++it)
                       {
                           if(it->end.x == current.x && it->end.y == current.y)
                           {
                               isSameSpot = true;
                               break;
                           }
                       }
                       if(!isSameSpot)
                       {
                           Step step = Step(startTile, current, captured);
                           move.push_back(step);
                           Piece p = Piece(current.x, current.y, piece.alliance, false);
                           if((p.alliance == Alliance::RED && p.y == 0)
                              || (p.alliance == Alliance::BLUE && p.y == BOARD_SIZE - 1))
                           {
                               p.crown();
                           }
                           calcAllJumps(p, move, legalMoves);
                       }
                    }
                }
                else
                {
                    if(current.hasPiece() && current.piece.alliance != piece.alliance)
                    {
                        enemyDetected = true;
                        captured = current.piece;
                    }
                }

                current = getTile(current.x + offsetX_[dir], current.y + offsetY_[dir]);
            }
            if(!move.empty())
            {
                legalMoves.push_back(move);
            }
        }
    }
}

/*
void Game::findAllJumps(int x, int y, Side side, Move move, std::vector<Move> &lolm) const
{
  const int dx[] = {-1, 1, 1, -1};
  const int dy[] = {-1, -1, 1, 1};
  for (int d = 0; d < 4; ++d)
  {
    const int xx = x + dx[d];
    const int yy = y + dy[d];
    if (xx < 0 || xx >= 8 || yy < 0 || yy >= 8)
      continue;
    if (board_[xx][yy] == EMPTY || board_[xx][yy] == static_cast<Cell>(side))
      continue;
    const int xxx = x + 2 * dx[d];
    const int yyy = y + 2 * dy[d];
    if (xxx < 0 || xxx >= 8 || yyy < 0 || yyy >= 8)
      continue;
    if (board_[xxx][yyy] == EMPTY)
    {
      bool samePlace = false;
      for (Move::iterator i = move.begin(); i != move.end(); ++i)
        if (i->second == Pos(xxx, yyy))
        {
          samePlace = true;
          break;
        }
      if (!samePlace)
      {
        move.push_back(Step(Pos(x, y), Pos(xxx, yyy)));
        lolm.push_back(move);
        findAllJumps(xxx, yyy, side, move, lolm);
      }
    }
  }
}
*/

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


