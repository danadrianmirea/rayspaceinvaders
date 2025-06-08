#include "globals.h"
#include "misteryship.h"

MysteryShip::MysteryShip()
{
    Image img = LoadImage("Graphics/mystery.png");
    ImageResize(&img, misteryShipSize, (int)((float)img.height * ((float)misteryShipSize / (float)img.width)));
    image = LoadTextureFromImage(img);
    UnloadImage(img);
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
        return {position.x, position.y, static_cast<float>(image.width * 1.0f), static_cast<float>(image.height * 1.0f)};
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
        position.x += speed * GetFrameTime() * targetFps;  // Scale with delta time and normalize to target FPS
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
        DrawTextureEx(image, position, 0.0f, 1.0f, WHITE);
    }
}

void MysteryShip::Destroy()
{
    alive = false;
    position.x = -1000;  // Move it far off screen
}