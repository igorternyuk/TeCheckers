#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>

struct Spot
{
    int x, y;
    Spot(int x = 0, int y = 0)
    {
        this->x = x;
        this->y = y;
    }
    int distanceTo(Spot spot) const;
};

class Board
{
public:
    explicit Board();
    enum
    {
        BOARD_SIZE = 8,
        NUM_CHECKER_ROW_FOR_ONE_SIDE = 3
    };

    enum Cell
    {
        EMPTY,
        RED_CHECKER,
        RED_KING,
        BLUE_CHECKER,
        BLUE_KING
    };

    void setupInitialPosition();
    bool isValidSpot(int x, int y) const;
    bool isValidSpot(Spot spot) const;
    bool isCellEmpty(int x, int y) const;
    Cell getCell(int x, int y) const;
    Cell getCell(Spot spot) const;

private:
    Cell mGrid[BOARD_SIZE][BOARD_SIZE];
    void clearBoard();
    bool setCell(Cell cell, Spot spot);
};

#endif // BOARD_HPP
