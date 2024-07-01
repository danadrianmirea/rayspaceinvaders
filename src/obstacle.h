#pragma once

#include <vector>
#include "block.h"

class Obstacle
{
public:
    Obstacle(Vector2 pos);
    void Draw();
    Vector2 position;
    std::vector<Block> blocks;
    static std::vector<std::vector<int>> grid;
};