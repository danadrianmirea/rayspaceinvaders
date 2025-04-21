#include "globals.h"
#include "spaceship.h"
#include <algorithm>
#include <iostream>

Spaceship::Spaceship()
{
    image = LoadTexture("Graphics/spaceship.png");
    laserSound = LoadSound("Sounds/laser.ogg");
    Reset();
}

Spaceship::~Spaceship()
{
    UnloadTexture(image);
    UnloadSound(laserSound);
#if MUSIC == 1
    UnloadMusicStream(music);
#endif
}

void Spaceship::Reset()
{
    position.x = (gameScreenWidth - image.width) / 2;
    position.y = gameScreenHeight - image.height - 100;
    lasers.clear();
    shipSpeed = cShipSpeed;
    laserSpeed = cLaserSpeed;
    laserFireTimer = 0;
    readyToFire = false;
    // Don't reset the fire rate here, it should be preserved
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
    DrawTextureEx(image, position, 0.0f, 1.0f, WHITE);

    for (auto &l : lasers)
    {
        l.Draw();
    }
}

void Spaceship::MoveLeft()
{
    float currentSpeed = shipSpeed;
    if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
    {
        currentSpeed *= speedBoostMultiplier;
    }
    position.x -= currentSpeed;
    if (position.x < frameOffsetLeft)
    {
        position.x = frameOffsetLeft;
    }
}

void Spaceship::MoveRight()
{
    float currentSpeed = shipSpeed;
    if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
    {
        currentSpeed *= speedBoostMultiplier;
    }
    position.x += currentSpeed;
    if (position.x > gameScreenWidth - image.width - frameOffsetRight)
    {
        position.x = gameScreenWidth - image.width - frameOffsetRight;
    }
}

void Spaceship::FireLaser()
{
    if (readyToFire)
    {
        lasers.push_back(Laser({position.x + image.width / 2 - Laser::laserWidth / 2, position.y}, laserSpeed));
        PlaySound(laserSound);
        readyToFire = false;
    }
}

Rectangle Spaceship::getRect()
{
    return {position.x, position.y, (float)image.width * 1.0f, (float)image.height * 1.0f};
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

void Spaceship::SetFireRate(float rate)
{
    laserFireInterval = rate;
    // Reset the timer to prevent any timing issues
    laserFireTimer = 0;
    readyToFire = true;
}
