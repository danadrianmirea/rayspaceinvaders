
#include "globals.h"
#include "laser.h"

Laser::Laser(Vector2 pos, int spd)
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
        if(position.y > gameScreenHeight || position.y < 0)
        {
            active = false;
        }
    }
}

void Laser::Draw()
{
    if (active)
    {
        DrawRectangle(position.x, position.y, laserWidth, laserHeight, laserColor);
    }
}

Rectangle Laser::getRect()
{
    return {position.x, position.y, 4.0f, 4.0f};
}
