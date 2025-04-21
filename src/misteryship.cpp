#include "globals.h"
#include "misteryship.h"

MysteryShip::MysteryShip()
{
    image = LoadTexture("Graphics/mystery.png");
    alive = false;
}

MysteryShip::~MysteryShip()
{
    UnloadTexture(image);
}

void MysteryShip::Spawn()
{
    position.y = 90;
    int side = GetRandomValue(0, 1);

    if(side == 0)
    {
        position.x = frameOffsetLeft;
        speed = 1;
    }
    else 
    {
        position.x = gameScreenWidth - image.width - frameOffsetRight;
        speed = -1;
    }
    alive = true;
}

Rectangle MysteryShip::getRect()
{
    if(alive)
    {
        return {position.x, position.y, (float)image.width * gameScale*0.8, (float)image.height * gameScale*0.8};
    }
    else
    {
        return {position.x, position.y, 0, 0};
    }
}

void MysteryShip::Update()
{
    
    if(alive)
    {
        position.x += speed;
        if(position.x > gameScreenWidth - image.width - frameOffsetRight || position.x < frameOffsetLeft)
        {
            alive = false;
        }
    }
}

void MysteryShip::Draw()
{
    if(alive)
    {
        DrawTextureEx(image, position, 0.0f, gameScale, WHITE);
    }
}