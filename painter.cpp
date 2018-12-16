#include "painter.hpp"
#include <cmath>

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
                if(currTile.piece.alliance == Board::Alliance::RED)
                {
                    drawPolygon(x * cellSize + cellSize / 2,
                                y * cellSize + cellSize / 2,
                                0.45f * cellSize, 5, {255, 0, 0});
                }
                else if(currTile.piece.alliance == Board::Alliance::BLUE)
                {
                    drawPolygon(x * cellSize + cellSize / 2,
                                y * cellSize + cellSize / 2,
                                0.45f * cellSize, 5, {24,41,235});
                }
                if(currTile.piece.isKing)
                {
                    drawPolygon(x * cellSize + cellSize / 2,
                                y * cellSize + cellSize / 2,
                                cellSize / 4, 20,
                                {255,255,0});
                }
            }
        }
    }
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
