#include "painter.hpp"
#include "game.hpp"
#include <GL/glut.h>
//#include <GL/gl.h>
#include <cmath>
#include <cstring>

Painter::Painter()
{}

void Painter::drawPolygon(float centerX, float centerY, float radius,
                          int numSides, Color color)
{
    glColor3f(color.red / 255.f, color.green / 255.f, color.blue / 255.f);
    glBegin(GL_POLYGON);
    for(int a = 0; a < numSides; ++a)
    {
        float x = centerX + radius * cos(2 * M_PI * a / numSides);
        float y = centerY + radius * sin(2 * M_PI * a / numSides);
        glVertex2f(x, y);
    }
    glEnd();
}

void Painter::drawEmptyBoard(int topLeftX, int topLeftY, int boardSize,
                        int side, Color darkCells, Color lightCells)
{
    for(int y = 0; y < boardSize; ++y)
    {
        for(int x = 0; x < boardSize; ++x)
        {
            Color color;
            if((y + 1) % 2 == 0)
            {
                color = x % 2 == 0 ? darkCells : lightCells;
            }
            else
            {
                color = x % 2 == 0 ? lightCells : darkCells;
            }
            drawFilledRect(topLeftX + x * side, topLeftY + y * side, side, color);
        }
    }
}

void Painter::drawBoard(const Board &board)
{
    const int cellSize = 60;
    drawEmptyBoard(0,0,Board::BOARD_SIZE, cellSize, {94,177,78}, {255,255,255});
    for(int y = 0; y < Board::BOARD_SIZE; ++y)
    {
        for(int x = 0; x < Board::BOARD_SIZE; ++x)
        {
            Board::Tile currTile = board.getTile(x,y);
            if(board.getTile(x,y).hasPiece())
            {
                const float cx = x * cellSize + cellSize / 2;
                const float cy = y * cellSize + cellSize / 2;

                if(currTile.piece.alliance == Board::Alliance::RED)
                {
                    drawPolygon(cx, cy, 0.45f * cellSize, 5, {255, 0, 0});
                }
                else if(currTile.piece.alliance == Board::Alliance::BLUE)
                {
                    drawPolygon(cx, cy,
                                0.45f * cellSize, 5, {0,109,211});
                }
                if(currTile.piece.isKing)
                {
                    float bottomLeftX = cx - cellSize / 6;
                    float bottomRightX = cx + cellSize / 6;
                    float bottomY = cy + cellSize / 6;
                    float topY = cy - cellSize / 4;
                    float leftX = bottomLeftX - cellSize / 12;
                    float rightX = bottomRightX + cellSize / 12;
                    float topSides = topY + cellSize / 12;

                    glColor3f(1.0f, 1.0f, 0.0f);
                    glBegin(GL_POLYGON);
                    glVertex2f(bottomLeftX, bottomY);
                    glVertex2f(leftX, topSides);
                    glVertex2f(bottomRightX, bottomY);
                    glEnd();

                    glColor3f(1.0f, 1.0f, 0.0f);
                    glBegin(GL_POLYGON);
                    glVertex2f(bottomLeftX, bottomY);
                    glVertex2f(cx, topY);
                    glVertex2f(bottomRightX, bottomY);
                    glEnd();

                    glColor3f(1.0f, 1.0f, 0.0f);
                    glBegin(GL_POLYGON);
                    glVertex2f(bottomLeftX, bottomY);
                    glVertex2f(rightX, topSides);
                    glVertex2f(bottomRightX, bottomY);
                    glEnd();


                }
            }
        }
    }
}

void Painter::drawMoveStep(const Board::Step &step, Color color)
{
    double x1 = step.start.x * Game::SIDE + Game::SIDE / 2;
    double y1 = step.start.y * Game::SIDE + Game::SIDE / 2;
    double x2 = step.end.x * Game::SIDE + Game::SIDE / 2;
    double y2 = step.end.y * Game::SIDE + Game::SIDE / 2;
    const double arrowSideLength = 20;
    double alpha = atan2(y2 - y1, x2 - x1);
    double leftX = x2 - arrowSideLength * cos(alpha - M_PI / 12);
    double leftY = y2 - arrowSideLength * sin(alpha - M_PI / 12);
    double rightX = x2 - arrowSideLength * cos(alpha + M_PI / 12);;
    double rightY = y2 - arrowSideLength * sin(alpha + M_PI / 12);;
    glColor3f(color.red / 255.0f, color.green / 255.0f, color.blue / 255.0f);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(leftX, leftY);
    glVertex2f(x2, y2);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(rightX, rightY);
    glVertex2f(x2, y2);
    glEnd();
    glLineWidth(1);
}

void Painter::drawFilledRect(float x, float y, float side, Color color)
{
    glColor3f(color.red / 255.f, color.green / 255.f, color.blue / 255.f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + side, y);
    glVertex2f(x + side, y + side);
    glVertex2f(x, y + side);
    glEnd();
}

void Painter::drawWord(char *word, float x_, float y_, float space, Color color)
{
    glColor3f(color.red / 255.f, color.green / 255.f, color.blue / 255.f);
    for(unsigned int i = 0; i < strlen(word); ++i)
    {
        glRasterPos2f(x_ + i * space, y_);
        char ch = word[i];
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (int)ch);
    }
}
