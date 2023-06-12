#include "game.hpp"
#include "utils.hpp"
#include <GL/glut.h>
#include <iostream>
#include <algorithm>
#include <iostream>

Game *Game::getInstance()
{
    static Game game;
    return &game;
}

Game::Game()
{}

Board::Alliance Game::getAiPlayer() const
{
    return _aiPlayer;
}

Board::Alliance Game::getCurrentPlayer() const
{
    return _turn;
}

Board::Alliance Game::getHumanPlayer() const
{
    return _humanPlayer;
}

const Board &Game::getBoard() const
{
    return _board;
}

void Game::onMouseClick(int x, int y)
{
    if(_board.GetGameStatus() != GameStatus::PLAY || _turn != _humanPlayer)
    {
        std::cout << "It is not your turn" << std::endl;
        return;
    }
    if(_selected.x == -1)
    {
        selectTile(x,y);
        std::cout << "Selecting the first tile" << std::endl;
    }
    else
    {
        std::cout << "Trying to move" << std::endl;
        MoveStatus moveStatus = tryToMove(x,y);
        if(moveStatus == MoveStatus::ILLEGAL_MOVE)
        {
            std::cout << "Illegal move" << std::endl;
            unselect();
        }
        else
        {
            auto log = _board.getMoveLog();
            lastMove_ = log.at(log.size() - 1);
            if(moveStatus == MoveStatus::REGULAR_MOVE || moveStatus == MoveStatus::SINGLE_JUMP)
            {
                switchTurn();
                checkGameStatus();
                unselect();

                if(_board.GetGameStatus() != GameStatus::PLAY)
                {
                    return;
                }                
                std::cout << "Move done" << std::endl;

            }
            else if(moveStatus == MoveStatus::JUMP_SEQUENCE)
            {
                std::cout << "Continue jumps" << std::endl;
                auto lastStep = lastMove_.at(lastMove_.size() - 1);
                selectTile(lastStep.end.x, lastStep.end.y);
            }
        }
    }
}

void Game::aiMove()
{
    std::string hash = getBoard().CalcHash();
    Board::Move bestMove;
    if(_mapBestMoves.find(hash) != _mapBestMoves.end())
        bestMove = _mapBestMoves[hash];
    else
    {
        bestMove = _ai.getBestMove(_board);
         _mapBestMoves[hash] = bestMove;
    }
    _board.makeMove(bestMove);
    lastMove_ = bestMove;
    switchTurn();
    checkGameStatus();
}

void Game::selectTile(int x, int y)
{
    if(_board.isValidTile(x,y))
    {
        Board::Tile tile = _board.getTile(x,y);
        if(tile.isDark() && tile.hasPiece()
                && tile.piece.alliance == this->_turn)
        {
            _selected = tile;
        }
    }
    else
    {
        unselect();
    }
}

void Game::unselect()
{
    _selected = Board::NULL_TILE;
    _selected.x = -1;
}

void Game::checkGameStatus()
{
    /*std::vector<Board::Move> legalMoves;
    board_.calcLegalMoves(this->turn_, legalMoves);
    if(legalMoves.empty())
    {
        if(this->turn_ == Board::Alliance::RED)
        {
            this->status_ = GameStatus::BLUE_WON;
        }
        else if(this->turn_ == Board::Alliance::BLUE)
        {
            this->status_ = GameStatus::RED_WON;
        }
    }*/
}

void Game::switchTurn()
{
    if(this->_turn == Board::Alliance::RED)
    {
        this->_turn = Board::Alliance::BLUE;
    }
    else if(this->_turn == Board::Alliance::BLUE)
    {
        this->_turn = Board::Alliance::RED;
    }
}

void Game::startNewGame()
{
    _board.Reset();
    //status_ = GameStatus::PLAY;
    _turn = Board::Alliance::RED;
    lastMove_.clear();
}

void Game::printBoard()
{
    std::cout << "//////////// BOARD ////////////" << std::endl;
    std::cout << _board.toString() << std::endl;
    std::cout << "///////////////////////////////" << std::endl;
}

Game::MoveStatus Game::tryToMove(int x, int y)
{
    if(_board.isValidTile(x,y))
    {
        Board::Tile endTile = _board.getTile(x,y);
        if(endTile.isLight())
        {
            return MoveStatus::ILLEGAL_MOVE;
        }
        std::vector<Board::Move> legalMoves;
        _board.calcLegalMoves(this->_turn, legalMoves);
        std::cout << "legalMoves.size() = " << legalMoves.size() << " for " << static_cast<int>(_turn) << std::endl;
        std::vector<Board::Move> filtred(legalMoves.size());
        auto it = std::copy_if(legalMoves.begin(), legalMoves.end(), filtred.begin(),
                     [&](const Board::Move & move){
                        Board::Step firstStep = move.at(0);
                        return firstStep.start == _selected && firstStep.end == endTile;
                     });
        filtred.resize(std::distance(filtred.begin(), it));
        if(!filtred.empty())
        {
            Board::Move move = filtred.at(0);
            if(_board.makeMove(move))
            {
                printBoard();
                std::cout << "DONE" << std::endl;
            }
            if(filtred.size() == 1)
            {
                return move.at(0).isJump()
                        ? MoveStatus::SINGLE_JUMP
                        : MoveStatus::REGULAR_MOVE;
            }
            else
            {
                return MoveStatus::JUMP_SEQUENCE;
            }
        }
        else
        {
            return MoveStatus::ILLEGAL_MOVE;
        }
    }
    return MoveStatus::ILLEGAL_MOVE;
}

void Game::run(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    int w = glutGet(GLUT_SCREEN_WIDTH);
    int h = glutGet(GLUT_SCREEN_HEIGHT);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition((w - WINDOW_WIDTH) / 2, (h - WINDOW_HEIGHT) / 2);
    glutCreateWindow("Draughts64");
    glClearColor(0,0,0,0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,WINDOW_WIDTH,WINDOW_HEIGHT,0,-1,1);    
    glutDisplayFunc(&Game::display);
    glutMouseFunc(&Game::mouse);    
    glutTimerFunc(500, &Game::timer, 0);
    glutKeyboardFunc(&Game::keyboardFunc);
    glutMainLoop();
}

void Game::display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    Utils::drawBoard(Game::getInstance()->getBoard());
    GameStatus status = Game::getInstance()->getBoard().GetGameStatus();
    if(status == GameStatus::PLAY)
        highlightLegalMoves();
    highlightLastMove();
    drawGameStatus();
    glutSwapBuffers();
    Board::Alliance turn = Game::getInstance()->_turn;
    if(turn == Board::Alliance::BLUE)
    {
        std::cout << "CPU starts thinking ..." << std::endl;
        Game::move_ai();
    }
}

void Game::timer(int)
{
    display();
    glutTimerFunc(500, timer, 0);
}

void Game::mouse(int button, int state, int x, int y)
{
    if(state == GLUT_UP && button == GLUT_LEFT_BUTTON)
    {
        int mx = x / SIDE;
        int my = y / SIDE;
        std::cout << "mx = " << mx << " my = " << my << std::endl;
        Game::getInstance()->onMouseClick(mx, my);
        /*if(Game::getInstance()->getAiPlayer() == Game::getInstance()->turn_)
        {
            Game::getInstance()->aiMove();
        }*/
    }
}

void Game::move_ai()
{
    if(Game::getInstance()->getAiPlayer() == Game::getInstance()->_turn)
    {
        Game::getInstance()->aiMove();
    }
}

void Game::keyboardFunc(unsigned char key, int x, int y)
{
    if(key == 13)
    {
        Game::getInstance()->startNewGame();
    }
    else if(key == ' ')
    {
        std::cout << "Last move undo" << std::endl;
        auto log = Game::getInstance()->_board.getMoveLog();
        if(!log.empty())
        {
            auto currAliance = log.at(log.size() - 1).at(0).end.piece.alliance;
            Game::getInstance()->_board.undoLastMove();
            Game::getInstance()->unselect();
            Game::getInstance()->_turn = currAliance == Board::Alliance::RED
                    ? Board::Alliance::BLUE
                    : Board::Alliance::RED;
        }
    }
}

void Game::highlightLastMove()
{
    Board::Move lastMove = Game::getInstance()->lastMove_;
    for(auto it = lastMove.begin(); it != lastMove.end(); ++it)
    {
        Utils::drawMoveStep(*it, {211,204,0});
    }
}

void Game::highlightLegalMoves()
{
    std::vector<Board::Move> lolm;
    Board::Alliance turn = Game::getInstance()->_turn;
    Game::getInstance()->_board.calcLegalMoves(turn, lolm);
    for(auto &m: lolm)
    {
        for(auto it = m.begin(); it != m.end(); ++it)
        {
            Utils::drawMoveStep(*it, {0,255,0});
        }
    }
}

void Game::drawGameStatus()
{
    GameStatus status = Game::getInstance()->getBoard().GetGameStatus();
    if(status == GameStatus::PLAY)
    {
        Board::Alliance turn = Game::getInstance()->_turn;
        if(turn == Board::Alliance::RED)
            Utils::drawWord("REDS TO PLAY", 5, 20, 20, {255,0,0});
        else if(turn == Board::Alliance::BLUE)
            Utils::drawWord("BLUES TO PLAY", 5, 20, 20, {0,0,255});
    }
    else if(status == GameStatus::RED_WON)
    {
        Utils::drawWord("RED PLAYER WON!", 5, 20, 20, {255,0,0});
    }
    else if(status == GameStatus::BLUE_WON)
    {
        Utils::drawWord("BLUE PLAYER WON!", 5, 20, 20, {0,0,255});
    }
    else if(status == GameStatus::DRAW)
    {
        Utils::drawWord("DRAW!", 5, 20, 20, {0,127,0});
    }
}
