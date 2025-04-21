#include "globals.h"
#include "laser.h"

Laser::Laser(Vector2 pos, float spd)
{
    position = pos;
    speed = spd;
    active = true;
}

void Laser::Update()
{
    position.y -= speed;

    if(active)
    {
        if(position.y > gameScreenHeight - 100 || position.y < frameOffsetLeft)
        {
            active = false;
        }
    }
}

void Laser::Draw()
{
    if (active)
    {
        DrawRectangle(position.x, position.y, laserWidth * 1.0f, laserHeight * 1.0f, laserColor);
    }
}

Rectangle Laser::getRect()
{
    return {position.x, position.y, 4.0f * 1.0f, 4.0f * 1.0f};
}
