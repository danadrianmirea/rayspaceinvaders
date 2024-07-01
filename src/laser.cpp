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
        if(position.y > GetScreenHeight() || position.y < 0)
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

bool Laser::IsActive()
{
    return active;
}
