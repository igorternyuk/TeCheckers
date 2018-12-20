#ifndef GAME_HPP
#define GAME_HPP

#include "board.hpp"
#include "alphabeta.hpp"
#include "painter.hpp"

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

    enum class GameStatus
    {
        RED_WON,
        BLUE_WON,
        DRAW,
        PLAY
    };
    enum class MoveStatus
    {
        REGULAR_MOVE,
        SINGLE_JUMP,
        JUMP_SEQUENCE,
        ILLEGAL_MOVE
    };
    Board board_;
    AlphaBeta ai_;
    Board::Alliance turn_ = Board::Alliance::RED;
    Board::Alliance humanPlayer_ = Board::Alliance::RED;
    Board::Alliance aiPlayer_ = Board::Alliance::BLUE;
    Board::Tile& selected_ = Board::NULL_TILE;
    Board::Move lastMove_;
    GameStatus status_ = GameStatus::PLAY;
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
    static void mouse(int state, int button, int x, int y);
    static void keyboardFunc(unsigned char key, int x, int y);
    static void highlightLastMove();
    static void highlightLegalMoves();
    static void drawGameStatus();
    static void drawMove(const Board::Move& move);
};



#endif // GAME_HPP
