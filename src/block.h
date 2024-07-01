#pragma once

#include "raylib.h"

class Block
{
public:
    Block(Vector2 pos);
    void Draw();
    static const int blockWidth;
    static const Color blockColor;

private:
    Vector2 position;
    
};