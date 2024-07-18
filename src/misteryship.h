#pragma once

#include "raylib.h"

class MysteryShip
{
public:
    MysteryShip();
    ~MysteryShip();
    void Update(float dt);
    void Draw();
    void Spawn();
    Rectangle getRect();
    bool alive;
private:
    Vector2 position;
    Texture2D image;
    int speed;
    const int cMysteryShipSpeed = 100;
};