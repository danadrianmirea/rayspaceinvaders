#pragma once

#include "raylib.h"

class Block
{
public:
    Block(Vector2 pos);
    void Draw(int row, int col);
    Rectangle getRect();
    static const int blockWidth;
    static const Color blockColor;  // Keep this for fallback if image fails
    static Image blockImage;  // Static image for all blocks
    static bool blockImageLoaded;

private:
    Vector2 position;
};