#include "globals.h"
#include "laser.h"

Texture2D Laser::laserTexture;
Texture2D Laser::alienRedLaserTexture;
Texture2D Laser::alienBlueLaserTexture;
Texture2D Laser::alienPurpleLaserTexture;

Laser::Laser(Vector2 pos, float spd, bool isPlayer)
{
    position = pos;
    speed = spd;
    active = true;
    isPlayerLaser = isPlayer;
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
        if (isPlayerLaser)
        {
            DrawTexture(laserTexture, position.x, position.y, WHITE);
        }
        else
        {
            DrawTexture(alienRedLaserTexture, position.x, position.y, WHITE);
        }
    }
}

Rectangle Laser::getRect()
{
    return {position.x, position.y, (float)laserTexture.width, (float)laserTexture.height};
}
