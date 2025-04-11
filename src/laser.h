#pragma once

#include "raylib.h"

class Laser
{
public:
    Laser(Vector2 pos, float spd);
    void Update();
    void Draw();
    bool active;
    Rectangle getRect();

    static const int laserWidth = 4;
    static const int laserHeight = 20;

private:
    Vector2 position;
    float speed;

    Color laserColor = YELLOW;

};