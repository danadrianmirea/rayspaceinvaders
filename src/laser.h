#pragma once

#include "raylib.h"

class Laser
{
public:
    Laser(Vector2 pos, int spd);
    void Update();
    void Draw();
    bool active;
    Rectangle getRect();

    static const int laserWidth = 4;
    static const int laserHeight = 20;

private:
    Vector2 position;
    int speed;

    Color laserColor = YELLOW;

};