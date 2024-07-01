#include "game.h"

Game::Game()
{
    CreateObstacles();
    CreateAliens();
}

Game::~Game()
{
}

void Game::Draw()
{
    spaceship.Draw();

    for(auto& obs : obstacles)
    {
        obs.Draw();
    }

    for(auto& alien : aliens)
    {
        alien.Draw();
    }
}

void Game::Update()
{
    spaceship.Update();
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
    for(int row = 0; row < numAlienRows; row++)
    {
        for(int column = 0; column < numAlienColumns; column++)
        {
            int alienType;
            if(row == 0)
            {
                alienType = 3;
            }
            else if(row ==1 || row == 2) 
            {
                alienType = 2;
            }
            else 
            {
                alienType = 1;
            }

            float x = alienOffsetX + column * alienCellSize;
            float y = alienOffsetY + row * alienCellSize;
            aliens.push_back(Alien(alienType, {x, y} ) );
        }
    }
}
