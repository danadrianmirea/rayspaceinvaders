#pragma once

#include "raylib.h"

class Laser
{
public:
    Laser(Vector2 pos, float spd, bool isPlayer);
    void Update();
    void Draw();
    bool active;
    Rectangle getRect();
    
    static Texture2D laserTexture;
    static Texture2D alienLaserTexture;

private:
    Vector2 position;
    float speed;
    bool isPlayerLaser;
    Color laserColor = YELLOW;

};