#ifndef GAME_HPP
#define GAME_HPP

#include "board.hpp"
#include "alphabeta.hpp"
#include "painter.hpp"

class Game
{    
public:
    static Game getInstance();
    void run(int argc, char *argv[]);
    const Board &getBoard() const;

private:
    explicit Game();

    enum
    {
        WINDOW_WIDTH = 480,
        WINDOW_HEIGHT = 480,
        SIDE = WINDOW_WIDTH / Board::BOARD_SIZE
    };
    Board board_;
    AlphaBeta ai_;
    Board::Alliance turn_ = Board::Alliance::RED;
    Board::Tile selected_;
    Painter painter;

    void onMouseClick(int x, int y);
    void selectTile(int x, int y);
    void unselect();
    bool tryToMove(int x, int y);
    static void display();
    static void mouse(int state, int button, int x, int y);
};



#endif // GAME_HPP
