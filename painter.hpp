#ifndef PAINTER_HPP
#define PAINTER_HPP

#include "board.hpp"

struct Color
{
    int red;
    int green;
    int blue;
    /*Color(int red = 0, int green = 0, int blue = 0)
    {
        if(red < 0) red = 0;
        if(red > 255) red = 255;
        if(green < 0) green = 0;
        if(green > 255) green = 255;
        if(blue < 0) blue = 0;
        if(blue > 255) blue = 255;
    }*/
};

class Painter
{
public:
    explicit Painter();
    static void drawFilledRect(float x, float y, float side, Color color = {255,255,255});
    static void drawPolygon(float centerX, float centerY, float radius, int numSides = 5,
                     Color color = {255,255,255});
    static void drawEmptyBoard(int topLeftX = 0, int topLeftY = 0, int boardSize = 8,
                   int side = 60, Color darkCells = {0,0,0},
                   Color lightCells = {255,255,255});
    static void drawBoard(const Board& board);
    static void drawWord(char *word, float x_, float y_, float space, Color color);
};

#endif // PAINTER_HPP
