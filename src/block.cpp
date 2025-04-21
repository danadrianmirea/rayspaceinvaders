#include "block.h"
#include "globals.h"

const int Block::blockWidth = 3;
const Color Block::blockColor = Color{243, 216, 63, 255};

Block::Block(Vector2 pos)
{
    position = pos;
}

void Block::Draw()
{
    DrawRectangle(position.x, position.y, blockWidth * 1.0f, blockWidth * 1.0f, blockColor);
}

Rectangle Block::getRect()
{
    Rectangle rect;
    rect.x = position.x;
    rect.y = position.y;
    rect.width = 3 * 1.0f;
    rect.height = 3 * 1.0f;
    return rect;
}
