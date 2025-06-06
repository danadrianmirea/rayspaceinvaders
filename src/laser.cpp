#include "globals.h"
#include "laser.h"

Texture2D Laser::laserTexture;
Texture2D Laser::alienRedLaserTexture;
Texture2D Laser::alienBlueLaserTexture;
Texture2D Laser::alienPurpleLaserTexture;

Laser::Laser(Vector2 pos, float spd, bool isPlayer, AlienType alienType)
    : position(pos), speed(spd), isPlayerLaser(isPlayer), alienType(alienType)
{
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
        if (isPlayerLaser)
        {
            DrawTexture(laserTexture, position.x, position.y, WHITE);
        }
        else
        {
            switch (alienType)
            {
                case AlienType::ALIEN_1:
                    DrawTexture(alienRedLaserTexture, position.x, position.y, WHITE);
                    break;
                case AlienType::ALIEN_2:
                    DrawTexture(alienBlueLaserTexture, position.x, position.y, WHITE);
                    break;
                case AlienType::ALIEN_3:
                    DrawTexture(alienPurpleLaserTexture, position.x, position.y, WHITE);
                    break;
            }
        }
    }
}

Rectangle Laser::getRect()
{
    return {position.x, position.y, (float)laserTexture.width, (float)laserTexture.height};
}
