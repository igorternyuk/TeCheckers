#include "game.hpp"
#include "painter.hpp"
#include <iostream>
#include <GL/glut.h>

static Painter painter;
Board board;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    painter.drawBoard(board);

#ifdef DEBUG
    std::vector<Board::Move> moves;
    Board::Move move;
    board.calcAllJumps(board.getTile(0,7).piece, move, moves);
    /*board.calcLegalMoves(Board::Alliance::BLUE, moves);*/
    std::cout << "moves.size() = " << moves.size() << std::endl;

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

void mouse(int button, int state, int x, int y)
{
    if(state == GLUT_UP && button == GLUT_LEFT_BUTTON)
    {
        std::cout << "x = " << (x / 60) << " y = " << (y / 60) << std::endl;
    }
}
/*
void mouse(int button, int state, int x, int y)
{
  if (state == GLUT_UP)
  {
    std::vector<Move> moves;
    game.getListOfLegalMoves(Game::WHITE_SIDE, moves);
    if (moves.empty())
      game = Game();
    const int xx = x / (480 / 8);
    const int yy = y / (480 / 8);
    if (game.selectedCell() == std::make_pair(-1, -1))
      game.selectCell(xx, yy);
    else
      game.move(xx, yy);
    glutPostRedisplay();
  }
}
*/

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
    glutMouseFunc(mouse);
    glutMainLoop();
    return 0;
}
