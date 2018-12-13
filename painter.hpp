#ifndef PAINTER_HPP
#define PAINTER_HPP

#include "board.hpp"
#include <GL/glut.h>

struct Color
{
    int red;
    int green;
    int blue;
};

class Painter
{
public:
    explicit Painter();
    void drawFilledRect(float x, float y, float side, Color color = {255,255,255});
    void drawPolygon(float centerX, float centerY, float radius, int numSides = 5,
                     Color color = {255,255,255});
    void drawEmptyBoard(int topLeftX = 0, int topLeftY = 0, int boardSize = 8,
                   int side = 60, Color darkCells = {0,0,0},
                   Color lightCells = {255,255,255});
    void drawBoard(const Board& board);
};

#endif // PAINTER_HPP
