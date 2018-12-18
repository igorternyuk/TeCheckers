#include "game.hpp"

int main(int argc, char *argv[])
{
    Game::getInstance()->run(argc, argv);
    return 0;
}


/*Game game;

enum
{
    WINDOW_WIDTH = 480,
    WINDOW_HEIGHT = 480,
    SIDE = WINDOW_WIDTH / Board::BOARD_SIZE
};

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    Painter::drawBoard(game.getBoard());
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
    if(state == GLUT_UP && button == GLUT_LEFT_BUTTON)
    {
        int mx = x / SIDE;
        int my = y / SIDE;
        std::cout << "mx = " << mx << " my = " << my << std::endl;
        game.printBoard();
        game.onMouseClick(mx, my);
        std::cout << " Mouse clicked " << std::endl;
        game.printBoard();
        glutPostRedisplay();
    }
}

void keyboardFunc(unsigned char key, int x, int y)
{
    if(key == 13)
    {
        std::cout << "Enter was pressed" << std::endl;
        game.startNewGame();
    }
}

int main(int argc, char *argv[])
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
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboardFunc);
    glutMainLoop();
}
*/
