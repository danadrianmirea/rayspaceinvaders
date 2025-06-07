#pragma once

#include "raylib.h"

class Laser
{
public:
    enum class AlienType {
        ALIEN_1,  // Red laser
        ALIEN_2,  // Blue laser
        ALIEN_3   // Purple laser
    };

    Laser(Vector2 pos, float spd, bool isPlayer, AlienType alienType = AlienType::ALIEN_1);
    void Update();
    void Draw();
    bool active;
    Rectangle getRect();
    Vector2 getPosition() const { return position; }
    
    static Texture2D laserTexture;
    static Texture2D alienRedLaserTexture;
    static Texture2D alienBlueLaserTexture;
    static Texture2D alienPurpleLaserTexture;

private:
    Vector2 position;
    float speed;
    bool isPlayerLaser;
    AlienType alienType;
    Color laserColor = YELLOW;

};