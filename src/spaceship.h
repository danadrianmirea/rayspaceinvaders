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
    const float laserFireInterval = 0.700f;

    const int cShipSpeed = 2;
    const int cLaserSpeed = 4;

    Sound laserSound;
};