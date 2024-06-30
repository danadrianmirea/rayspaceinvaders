#include "spaceship.h"

Spaceship::Spaceship()
{
    image = LoadTexture("Graphics/spaceship.png");
    position.x = 100;
    position.y = 100;
}

Spaceship::~Spaceship()
{
    UnloadTexture(image);
}

void Spaceship::Update()
{
}

void Spaceship::Draw()
{
    DrawTextureV(image, position, WHITE);
}

void Spaceship::MoveLeft()
{
}

void Spaceship::MoveRight()
{
}

void Spaceship::FireLaser()
{
}
