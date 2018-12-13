#include <iostream>
#include <GL/glut.h>
#include "game.hpp"
#include "painter.hpp"

static Painter painter;
Board board;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    painter.drawBoard(board);
    glutSwapBuffers();
}

/*
void display()
{
  glClear(GL_COLOR_BUFFER_BIT);
  for (int x = 0; x < 8; ++x)
    for (int y = 0; y < 8; ++y)
    {
      if ((x + y) % 2)
        glColor3f(0.0f, 0.0f, 0.0f);
      else
        glColor3f(1.0f, 1.0f, 1.0f);
      glBegin(GL_QUADS);
      glVertex2f(x * 480 / 8, y * 480 / 8);
      glVertex2f((x + 1) * 480 / 8, y * 480 / 8);
      glVertex2f((x + 1) * 480 / 8, (y + 1) * 480 / 8);
      glVertex2f(x * 480 / 8, (y + 1) * 480 / 8);
      glEnd();
      switch (game.cell(x, y))
      {
      case Game::EMPTY:
        break;
      case Game::BLACK:
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_POLYGON);
        for (int a = 0; a < 5; ++a)
        {
          float xx = 480 / 8 / 2 * cos(2 * 3.1415926 * a / 5) + (x + 0.5f) * 480 / 8;
          float yy =  480 / 8 / 2 * sin(2 * 3.1415926 * a / 5) + (y + 0.5f) * 480 / 8;
          glVertex2f(xx, yy);
        }
        glEnd();
        break;
      case Game::WHITE:
        Pos selected = game.selectedCell();
        if (selected.first == x && selected.second == y)
          glColor3f(1.0f, 0.0f, 0.0f);
        else
          glColor3f(0.7f, 0.7f, 0.7f);
        glBegin(GL_POLYGON);
        for (int a = 0; a < 5; ++a)
        {
          float xx = 480 / 8 / 2 * cos(2 * 3.1415926 * a / 5) + (x + 0.5f) * 480 / 8;
          float yy =  480 / 8 / 2 * sin(2 * 3.1415926 * a / 5) + (y + 0.5f) * 480 / 8;
          glVertex2f(xx, yy);
        }
        glEnd();
        break;
      }
    }
#ifdef DEBUG
  std::vector<Move> moves;
  game.getListOfLegalMoves(Game::WHITE_SIDE, moves);
  glColor3f(0.0f, 1.0f, 0.0f);
  glBegin(GL_LINES);
  for (std::vector<Move>::iterator i = moves.begin(); i != moves.end(); ++i)
  {
    for (Move::iterator j = i->begin(); j != i->end(); ++j)
    {
      int x1 = j->first.first;
      int y1 = j->first.second;
      int x2 = j->second.first;
      int y2 = j->second.second;
      glVertex2f((x1 + 0.5f) * 480 / 8, (y1 + 0.5f) * 480 / 8);
      glVertex2f((x2 + 0.5f) * 480 / 8, (y2 + 0.5f) * 480 / 8);
    }
  }
#endif
  glutSwapBuffers();
}
*/

void timer(int = 0)
{

}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(480, 480);
    glutInitWindowPosition((1366 - 480) / 2, (768 - 480) / 2);
    glutCreateWindow("Checkers!");
    glClearColor(0,0,0,0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,480,480,0,-1,1);
    glutDisplayFunc(display);
    glutTimerFunc(300, timer, 0);
    glutMainLoop();
    return 0;
}
