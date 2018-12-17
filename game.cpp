#include "game.hpp"
#include <GL/glut.h>
#include <iostream>
#include <functional>

Game Game::getInstance()
{
    static Game game;
    return game;
}

Game::Game()
{

}

const Board &Game::getBoard() const
{
    return board_;
}

void Game::onMouseClick(int x, int y)
{

}

void Game::selectTile(int x, int y)
{

}

void Game::unselect()
{

}

bool Game::tryToMove(int x, int y)
{
    return false;
}

/*
std::vector<Move> listOfLegalMoves;
  getListOfLegalMoves(WHITE_SIDE, listOfLegalMoves);
  if (listOfLegalMoves.empty())
  {
    *this = Game();
    return;
  }

  Step step = Step(Pos(selectedCellX_, selectedCellY_), Pos(x, y));

  bool moved = false;

  for (std::vector<Move>::iterator i = listOfLegalMoves.begin(); i != listOfLegalMoves.end(); ++i)
  {
    if (step == i->front() && (!isAfterJump_ || (isAfterJump_ && abs(selectedCellX_ - x) == 2)))
    {
      moved = true;
      board_[selectedCellX_][selectedCellY_] = EMPTY;
      board_[x][y] = WHITE;
      if (abs(selectedCellX_ - x) == 2)
      {
        board_[(selectedCellX_ + x) / 2][(selectedCellY_ + y) / 2] = EMPTY;
        selectedCellX_ = x;
        selectedCellY_ = y;
        isAfterJump_ = true;
        return;
      }
      else
        isAfterJump_ = false;
      break;
    }
  }

  if (isAfterJump_ || moved)
  {
    isAfterJump_ = false;
    blackMove();
  }
  selectedCellX_ = -1;
  selectedCellY_ = -1;
*/

void Game::run(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition((1366 - WINDOW_WIDTH) / 2, (768 - WINDOW_HEIGHT) / 2);
    glutCreateWindow("Checkers");
    glClearColor(0,0,0,0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,WINDOW_WIDTH,WINDOW_HEIGHT,0,-1,1);
    glutDisplayFunc(&Game::display);
    glutMouseFunc(&Game::mouse);
    glutMainLoop();
}

void Game::display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    Painter::drawBoard(Game::getInstance().getBoard());

#ifdef DEBUG
    std::vector<Board::Move> moves;
    Board::Move move;

    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    for(auto mi = moves.begin(); mi != moves.end(); ++mi)
    {
        for(auto si = mi->begin(); si != mi->end(); ++si)
        {
            int startX = si->start.x * 60 + 30;
            int startY = si->start.y * 60 + 30;
            int endX = si->end.x * 60 + 30;
            int endY = si->end.y * 60 + 30;
            glVertex2f(startX, startY);
            glVertex2f(endX, endY);
        }
    }
    glEnd();
#endif
    glutSwapBuffers();
}

void Game::mouse(int button, int state, int x, int y)
{
    if(state == GLUT_UP && button == GLUT_LEFT_BUTTON)
    {
        int mx = x / SIDE;
        int my = y / SIDE;
        std::cout << "mx = " << mx << " my = " << my << std::endl;
        Game::getInstance().onMouseClick(mx, my);
        glutPostRedisplay();
    }
}
