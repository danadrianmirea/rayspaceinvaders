#include "alien.h"
#include "globals.h"

Texture2D Alien::alienImages[3] = {};

Alien::Alien(int typeId, Vector2 pos)
{
    type = typeId;
    position = pos;

    if (alienImages[type - 1].id == 0)
    {
        Image img;
        switch (type)
        {
        case 1:
            img = LoadImage("Graphics/alien_1.png");
            ImageResize(&img, alienSize, (int)((float)img.height * ((float)alienSize / (float)img.width)));
            alienImages[0] = LoadTextureFromImage(img);
            UnloadImage(img);
            break;
        case 2:
            img = LoadImage("Graphics/alien_2.png");
            ImageResize(&img, alienSize, (int)((float)img.height * ((float)alienSize / (float)img.width)));
            alienImages[1] = LoadTextureFromImage(img);
            UnloadImage(img);
            break;
        case 3:
            img = LoadImage("Graphics/alien_3.png");
            ImageResize(&img, alienSize, (int)((float)img.height * ((float)alienSize / (float)img.width)));
            alienImages[2] = LoadTextureFromImage(img);
            UnloadImage(img);
            break;
        default:
            img = LoadImage("Graphics/alien_1.png");
            ImageResize(&img, alienSize, (int)((float)img.height * ((float)alienSize / (float)img.width)));
            alienImages[0] = LoadTextureFromImage(img);
            UnloadImage(img);
            break;
        }
    }
}

void Alien::Update(float speed, int direction)
{
    position.x += speed*direction;
}

void Alien::Draw()
{
    DrawTextureEx(alienImages[type-1], position, 0.0f, 1.0f, WHITE);
}

int Alien::GetType()
{
    return type;
}

void Alien::UnloadImages()
{
    for(int i=0; i<4; i++)
    {
        UnloadTexture(alienImages[i]);
    }
}

Rectangle Alien::getRect()
{
    return {position.x, position.y, (float)(alienImages[type-1].width) * 1.0f, (float)(alienImages[type-1].height) * 1.0f};
}
