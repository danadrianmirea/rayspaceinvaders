#include "block.h"
#include "globals.h"

const int Block::blockWidth = 3;
const Color Block::blockColor = Color{243, 216, 63, 255};
Image Block::blockImage;
bool Block::blockImageLoaded = false;

Block::Block(Vector2 pos)
{
    position = pos;

    // Load image if not already loaded
    if (!blockImageLoaded)
    {
        blockImage = LoadImage("Graphics/obstacle.png");
        blockImageLoaded = true;
    }
}

void Block::Draw(int row, int col)
{
    if (!isValid) return;  // Skip drawing if block is invalid

    if (blockImage.data)
    {
        Color pixelColor = GetImageColor(blockImage, col, row);
        DrawRectangle(position.x, position.y, blockWidth * 1.0f, blockWidth * 1.0f, pixelColor);
    }
    else
    {
        // Fallback to solid color if image is not loaded
        DrawRectangle(position.x, position.y, blockWidth * 1.0f, blockWidth * 1.0f, blockColor);
    }
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
