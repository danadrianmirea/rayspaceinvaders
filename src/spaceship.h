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
    void Update();
    void Draw();
    void MoveLeft();
    void MoveRight();
    void FireLaser();
    Rectangle getRect();
    void SetFireRate(float rate);
    float GetFireRate() const { return laserFireInterval; }

    std::vector<Laser> lasers;
    
private:
    void DeleteInactiveLasers();
    void ReloadWeapon();
    Texture2D image;
    Vector2 position;
    float shipSpeed;
    int laserSpeed;
    bool readyToFire;
    float laserFireTimer;
    float laserFireInterval;

    const float cShipSpeed = 1.4f;
    const int cLaserSpeed = 4;
    const float speedBoostMultiplier = 2.0f;  // Speed multiplier when shift is pressed
    const float cDefaultFireRate = 0.350f;    // Default fire rate

    Sound laserSound;
};