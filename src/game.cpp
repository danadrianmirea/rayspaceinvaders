#include "game.h"
#include <algorithm>

Game::Game()
{
    CreateObstacles();
    CreateAliens();
    aliensDirection = 1;
    alienLaserSpeed = -3;
    aliensReadyToFire = false;
    alienFireTimer = 0.0f;
    paused = false;
}

Game::~Game()
{
    Alien::UnloadImages();
}

void Game::Draw()
{
    spaceship.Draw();
    misteryShip.Draw();

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
    if (paused == false)
    {
        spaceship.Update();
        misteryShip.Update();

        MoveAliens();
        AlienShootLaser();

        for (auto &alienLaser : alienLasers)
        {
            alienLaser.Update();
        }

        DeleteInactiveAlienLasers();

        CheckForCollisions();
    }
}

void Game::DeleteInactiveAlienLasers()
{
    auto it = std::remove_if(alienLasers.begin(), alienLasers.end(), [](Laser &l)
                             { return l.active == false; });
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
    if (aliens.size() > 0)
    {
        if (aliensReadyToFire)
        {
            int randomIndex = GetRandomValue(0, aliens.size() - 1);
            Alien &alien = aliens[randomIndex];
            alienLasers.push_back(Laser({alien.position.x + alien.alienImages[alien.type - 1].width / 2, alien.position.y + alien.alienImages[alien.type - 1].height}, alienLaserSpeed));
            aliensReadyToFire = false;
            alienFireTimer = 0.0f;
        }
        else
        {
            alienFireTimer += GetFrameTime();
            if (alienFireTimer >= alienFireRate)
            {
                aliensReadyToFire = true;
            }
        }
    }
}

void Game::CheckForCollisions()
{
    // Spaceship lasers
    for (auto &laser : spaceship.lasers)
    {
        auto it = aliens.begin();
        while (it != aliens.end())
        {
            if (CheckCollisionRecs(it->getRect(), laser.getRect()))
            {
                it = aliens.erase(it);
                laser.active = false;
                break;
            }
            it++;
        }
        if (laser.active == false)
        {
            continue;
        }
    }
}

void Game::CreateObstacles()
{
    int obstacleWidth = Obstacle::grid[0].size() * Block::blockWidth;
    float gap = (gameScreenWidth - (4 * obstacleWidth)) / 5;

    for (int i = 0; i < 4; i++)
    {
        float offsetX = (i + 1) * gap + i * obstacleWidth;
        obstacles.push_back(Obstacle({offsetX, float(gameScreenHeight - 100)}));
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
        if (alien.position.x + alien.alienImages[alien.type - 1].width > gameScreenWidth - 1)
        {
            aliensDirection = -1;
            MoveDownAliens(2);
        }
        if (alien.position.x < 0)
        {
            aliensDirection = 1;
            // MoveDownAliens(2);
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
