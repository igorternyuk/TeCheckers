#include <iostream>
#include <GL/glut.h>

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();
}

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
