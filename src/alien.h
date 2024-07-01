#pragma once

#include "raylib.h"

class Alien
{
public:
    Alien(int typeId, Vector2 pos);
    void Update(int direction);
    void Draw();
    int GetType();
    static Texture2D alienImages[3];
    static void UnloadImages();
    int type;
    Vector2 position;

private:
};