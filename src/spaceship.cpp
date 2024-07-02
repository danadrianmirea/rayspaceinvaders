
#include "globals.h"
#include "spaceship.h"
#include <algorithm>
#include <iostream>

Spaceship::Spaceship()
{
    image = LoadTexture("Graphics/spaceship.png");
    position.x = (gameScreenWidth - image.width) / 2;
    position.y = gameScreenHeight - image.height;
    shipSpeed = 7;
    laserSpeed = 6;
    laserFireTimer = 0;
    readyToFire = false;
}

Spaceship::~Spaceship()
{
    UnloadTexture(image);
}

void Spaceship::Update()
{
    for (auto &l : lasers)
    {
        if (l.active)
        {
            l.Update();
        }
    }

    DeleteInactiveLasers();
    ReloadWeapon();
}

void Spaceship::Draw()
{
    DrawTextureV(image, position, WHITE);

    for (auto &l : lasers)
    {
        l.Draw();
    }
}

void Spaceship::MoveLeft()
{
    position.x -= shipSpeed;
    if (position.x < 0)
    {
        position.x = 0;
    }
}

void Spaceship::MoveRight()
{
    position.x += shipSpeed;
    if (position.x > gameScreenWidth - image.width)
    {
        position.x = gameScreenWidth - image.width;
    }
}

void Spaceship::FireLaser()
{
    if (readyToFire)
    {
        lasers.push_back(Laser({position.x + image.width / 2 - Laser::laserWidth / 2, position.y}, laserSpeed));
        readyToFire = false;
    }
}

Rectangle Spaceship::getRect()
{
    return {position.x, position.y, (float)image.width, (float)image.height};
}

void Spaceship::DeleteInactiveLasers()
{
    auto it = std::remove_if(lasers.begin(), lasers.end(), [](Laser &l)
                             { return l.active == false; });
    lasers.erase(it, lasers.end());
    // std::cout << "lasers size : " << lasers.size() << "\n";
}

void Spaceship::ReloadWeapon()
{
    if (readyToFire == false)
    {
        laserFireTimer += GetFrameTime();
        if (laserFireTimer >= laserFireInterval)
        {
            readyToFire = true;
            laserFireTimer = 0.0f;
        }
    }
}
