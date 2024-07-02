#include "globals.h"
#include "misteryship.h"

MisteryShip::MisteryShip()
{
    image = LoadTexture("Graphics/mystery.png");
    alive = false;
}

MisteryShip::~MisteryShip()
{
    UnloadTexture(image);
}

void MisteryShip::Spawn()
{
    position.y = 90;
    int side = GetRandomValue(0, 1);

    if(side == 0)
    {
        position.x = 0;
        speed = 3;
    }
    else 
    {
        position.x = gameScreenWidth - image.width;
        speed = -3;
    }
    alive = true;
}

Rectangle MisteryShip::getRect()
{
    if(alive)
    {
        return {position.x, position.y, (float)image.width, (float)image.height};
    }
    else
    {
        return {position.x, position.y, 0, 0};
    }
}

void MisteryShip::Update()
{
    
    if(alive)
    {
        position.x += speed;
        if(position.x > gameScreenWidth - image.width || position.x < 0)
        {
            alive = false;
        }
    }
}

void MisteryShip::Draw()
{
    if(alive)
    {
        DrawTextureV(image, position, WHITE);
        DrawRectangle(position.x, position.y, 50, 50, WHITE);
    }
}