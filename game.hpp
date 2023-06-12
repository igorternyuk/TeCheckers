#pragma once

#include "board.hpp"
#include "alphabeta.hpp"

class Game
{    
public:
    enum
    {
        WINDOW_WIDTH = 480,
        WINDOW_HEIGHT = 480,
        SIDE = WINDOW_WIDTH / Board::BOARD_SIZE
    };
    static Game* getInstance();
    void run(int argc, char *argv[]);

    Board::Alliance getHumanPlayer() const;
    Board::Alliance getAiPlayer() const;
    Board::Alliance getCurrentPlayer() const;

private:
    explicit Game();
    Game(const Game& game) = delete;
    Game(Game&& game) = delete;
    Game& operator=(const Game& game) = delete;
    Game& operator=(Game&& game) = delete;

    enum class MoveStatus
    {
        REGULAR_MOVE,
        SINGLE_JUMP,
        JUMP_SEQUENCE,
        ILLEGAL_MOVE
    };

private:

    const Board &getBoard() const;
    void selectTile(int x, int y);
    void unselect();
    MoveStatus tryToMove(int x, int y);
    void checkGameStatus();
    void switchTurn();
    void startNewGame();
    void printBoard();
    void onMouseClick(int x, int y);
    void aiMove();
    static void display();
    static void timer(int);
    static void mouse(int state, int button, int x, int y);
    static void move_ai();
    static void keyboardFunc(unsigned char key, int x, int y);
    static void highlightLastMove();
    static void highlightLegalMoves();
    static void drawGameStatus();
    static void drawMove(const Board::Move& move);

private:

    Board _board;
    AlphaBeta _ai;
    Board::Alliance _turn = Board::Alliance::RED;
    Board::Alliance _humanPlayer = Board::Alliance::RED;
    Board::Alliance _aiPlayer = Board::Alliance::BLUE;
    Board::Tile& _selected = Board::NULL_TILE;
    Board::Move lastMove_;
    std::map<std::string, Board::Move> _mapBestMoves;
};
