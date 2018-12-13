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

struct Step
{
    Spot start, end;
};

using Move = std::vector<Step>;

class Board
{
public:
    explicit Board();
    enum
    {
        BOARD_SIZE = 8,
        NUM_CHECKER_ROW_FOR_ONE_SIDE = 3
    };

    enum class Alliance
    {
        WHITE,
        BLACK
    };
    enum Cell
    {
        EMPTY,
        WHITE_CHECKER,
        WHITE_KING,
        BLACK_CHECKER,
        BLACK_KING
    };

    void setupInitialPosition();
    bool tryToMove(const Move& move);
    bool isValidSpot(int x, int y) const;
    bool isValidSpot(Spot spot) const;
    Cell getCell(int x, int y) const;
    Cell getCell(Spot spot) const;
private:
    Cell mGrid[BOARD_SIZE][BOARD_SIZE];
    void clearBoard();
    bool setCell(Cell cell, Spot spot);
};

/*
typedef std::pair<int, int> Pos;
typedef std::pair<Pos, Pos> Step;
typedef std::vector<Step> Move;

class Game
{
public:
  enum Side { BLACK_SIDE = 1, WHITE_SIDE = 2 };
  enum Cell { EMPTY, BLACK = BLACK_SIDE, WHITE = WHITE_SIDE };
  Game();
  Cell cell(int x, int y) const;
  Pos selectedCell() const;
  void selectCell(int x, int y);
  void unselect();
  void move(int x, int y);
  void getListOfLegalMoves(Side, std::vector<Move> &) const;
private:
  typedef Cell Board[8][8];
  Board board_;
  int selectedCellX_;
  int selectedCellY_;
  bool isAfterJump_;
  void findAllJumps(int x, int y, Side side, Move move, std::vector<Move> &listOfLegalMoves) const;
  bool blackMove(int level = 6);
  bool whiteMove(int level = 6);
  int score() const;
};
*/
#endif // BOARD_HPP
