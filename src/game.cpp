#include "game.h"
#include <algorithm>

Game::Game()
{
    CreateObstacles();
    CreateAliens();
    aliensDirection = 1;
    aliensReadyToFire = false;
    alienFireTimer = 0.0f;
}

Game::~Game()
{
    Alien::UnloadImages();
}

void Game::Draw()
{
    spaceship.Draw();

    for (auto &obs : obstacles)
    {
        obs.Draw();
    }

    for (auto &alien : aliens)
    {
        alien.Draw();
    }

    for (auto &alienLaser : alienLasers)
    {
        alienLaser.Draw();
    }
}

void Game::Update()
{
    spaceship.Update();
    MoveAliens();
    AlienShootLaser();

    for(auto& alienLaser : alienLasers)
    {
        alienLaser.Update();
    }

    DeleteInactiveAlienLasers();
}

void Game::DeleteInactiveAlienLasers()
{
    auto it = std::remove_if(alienLasers.begin(), alienLasers.end(), [](Laser &l)
                             { return l.IsActive() == false; });
    alienLasers.erase(it, alienLasers.end());
}

void Game::HandleInput()
{
    if (IsKeyDown(KEY_LEFT))
    {
        spaceship.MoveLeft();
    }
    else if (IsKeyDown(KEY_RIGHT))
    {
        spaceship.MoveRight();
    }

    if (IsKeyDown(KEY_SPACE))
    {
        spaceship.FireLaser();
    }
}

void Game::AlienShootLaser()
{
    if (aliensReadyToFire)
    {
        int randomIndex = GetRandomValue(0, aliens.size() - 1);
        Alien &alien = aliens[randomIndex];
        alienLasers.push_back(Laser({alien.position.x + alien.alienImages[alien.type-1].width / 2, alien.position.y + alien.alienImages[alien.type-1].height}, -6));
        aliensReadyToFire = false;
        alienFireTimer = 0.0f;
    }
    else 
    {
        alienFireTimer += GetFrameTime();
        if(alienFireTimer >= alienFireRate)
        {
            aliensReadyToFire = true;
        }
    }
}

void Game::CreateObstacles()
{
    int obstacleWidth = Obstacle::grid[0].size() * Block::blockWidth;
    float gap = (GetScreenWidth() - (4 * obstacleWidth)) / 5;

    for (int i = 0; i < 4; i++)
    {
        float offsetX = (i + 1) * gap + i * obstacleWidth;
        obstacles.push_back(Obstacle({offsetX, float(GetScreenHeight() - 100)}));
    }
}

void Game::CreateAliens()
{
    for (int row = 0; row < numAlienRows; row++)
    {
        for (int column = 0; column < numAlienColumns; column++)
        {
            int alienType;
            if (row == 0)
            {
                alienType = 3;
            }
            else if (row == 1 || row == 2)
            {
                alienType = 2;
            }
            else
            {
                alienType = 1;
            }

            float x = alienOffsetX + column * alienCellSize;
            float y = alienOffsetY + row * alienCellSize;
            aliens.push_back(Alien(alienType, {x, y}));
        }
    }
}

void Game::MoveAliens()
{
    for (auto &alien : aliens)
    {
        if (alien.position.x + alien.alienImages[alien.type - 1].width > GetScreenWidth())
        {
            aliensDirection = -1;
            MoveDownAliens(4);
        }
        if (alien.position.x < 0)
        {
            aliensDirection = 1;
            MoveDownAliens(4);
        }
        alien.Update(aliensDirection);
    }
}

void Game::MoveDownAliens(int distance)
{
    for (auto &alien : aliens)
    {
        alien.position.y += distance;
    }
}
