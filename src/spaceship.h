#pragma once

#include <vector>
#include "raylib.h"
#include "laser.h"

class Spaceship
{
public:
    Spaceship();
    ~Spaceship();
    void Reset();
    void Update(float dt);
    void Draw();
    void MoveLeft(float dt);
    void MoveRight(float dt);
    void FireLaser();
    Rectangle getRect();
    Texture2D& GetSpaceshipImage();

    std::vector<Laser> lasers;
    
private:
    void DeleteInactiveLasers();
    void ReloadWeapon();
    Texture2D image;
    Vector2 position;
    int shipSpeed;
    int laserSpeed;
    bool readyToFire;
    float laserFireTimer;
    const float laserFireInterval = 0.350f;

    const int cShipSpeed = 400;
    const int cLaserSpeed = 400;

    Sound laserSound;
};