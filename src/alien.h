#pragma once

#include "raylib.h"

class Alien
{
public:
    Alien(int typeId, Vector2 pos);
    void Update();
    void Draw();
    int GetType();
    Texture2D image;
    int type;
    Vector2 position;

private:
};