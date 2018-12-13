#include "board.hpp"
#include <cmath>
#include <algorithm>

Board::Board()
{

}

void Board::setupInitialPosition()
{
    clearBoard();
    for(int y = 0; y < NUM_CHECKER_ROW_FOR_ONE_SIDE; ++y)
    {
        for(int x = (y + 1) % 2; x < BOARD_SIZE; x += 2)
        {
            setCell(Cell::BLACK_CHECKER, {x,y});
        }
    }

    for(int y = 0; y < BOARD_SIZE - NUM_CHECKER_ROW_FOR_ONE_SIDE; ++y)
    {
        for(int x = (y + 1) % 2; x < BOARD_SIZE; x += 2)
        {
            setCell(Cell::WHITE_CHECKER, {x,y});
        }
    }
}

bool Board::tryToMove(const Move &move)
{
    return false;
}

bool Board::isValidSpot(int x, int y) const
{
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

bool Board::isValidSpot(Spot spot) const
{
    return isValidSpot(spot.x, spot.y);
}

Board::Cell Board::getCell(int x, int y) const
{
    return isValidSpot(x, y) ? mGrid[y][x] : EMPTY;
}

Board::Cell Board::getCell(Spot spot) const
{
    return getCell(spot.x, spot.y);
}

void Board::clearBoard()
{
    for(size_t y = 0; y < BOARD_SIZE; ++y)
    {
        for(size_t x = 0; x < BOARD_SIZE; ++x)
        {
            mGrid[y][x] = EMPTY;
        }
    }
}

bool Board::setCell(Board::Cell cell, Spot spot)
{
    if(isValidSpot(spot))
    {
        mGrid[spot.y][spot.x] = cell;
        return true;
    }
    return false;
}

int Spot::distanceTo(Spot spot) const
{
    const int dx = abs(this->x - spot.x);
    const int dy = abs(this->y - spot.y);
    return std::max(dx, dy);
}
