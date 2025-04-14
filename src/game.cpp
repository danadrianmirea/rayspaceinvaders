#include "game.h"
#include <algorithm>
#include <fstream>

#include <iostream>

Game::Game()
{
    explosionSound = LoadSound("Sounds/explosion.ogg");
#if MUSIC == 1
    music = LoadMusicStream("Sounds/music.ogg");
    PlayMusicStream(music);
#endif
    InitGame();
    
}

Game::~Game()
{
    Alien::UnloadImages();
#if MUSIC == 1
    UnloadMusicStream(music);
#endif
    UnloadSound(explosionSound);
}

void Game::InitGame()
{
    // Clear all existing game objects
    obstacles.clear();
    aliens.clear();
    alienLasers.clear();
    spaceship.lasers.clear();
    
    // Reset game state variables
    aliensDirection = 1;
    alienLaserSpeed = -3;
    alienShipSpeed = 0.4f;
    aliensReadyToFire = false;
    alienFireTimer = 0.0f;
    paused = false;
    lostWindowFocus = false;
    isInExitMenu = false;
    lostLife = false;
    lostLifeTimer = 0.0f;
    alienUpdateTimer = 0.0f;
    alienUpdateTimerExpired = false;
    lives = 3;
    score = 0;
    currentLevel = 1;  // Start at level 1
    highScore = LoadHighScoreFromFile();
    gameOver = false;
    timeLastMysteryShipSpawn = 0.0f;
    mysteryShipSpawnInterval = GetRandomValue(10, 20);
    isFirstFrameAfterReset = true;

    // Reset spaceship position and state
    spaceship.Reset();
    
    // Create new game objects
    CreateObstacles();
    CreateAliens();
    
    // Initialize player's fire rate
    float baseFireRate = 0.350f;
    float newFireRate = baseFireRate * 1.67f;  // Start with base fire rate at 60% speed
    spaceship.SetFireRate(newFireRate);
}

void Game::Reset()
{
    // Store the current fire rate before resetting
    float currentFireRate = spaceship.GetFireRate();
    
    obstacles.clear();
    aliens.clear();
    alienLasers.clear();
    spaceship.Reset();
    
    // Don't call InitGame() here as it would reset the level and fire rate
    // Instead, just recreate the game elements
    CreateObstacles();
    CreateAliens();
    aliensDirection = 1;
    aliensReadyToFire = false;
    alienFireTimer = 0.0f;
    paused = false;
    lostWindowFocus = false;
    isInExitMenu = false;
    lostLife = false;
    lostLifeTimer = 0.0f;
    alienUpdateTimer = 0.0f;
    alienUpdateTimerExpired = false;
    timeLastMysteryShipSpawn = 0.0f;
    mysteryShipSpawnInterval = GetRandomValue(10, 20);
    isFirstFrameAfterReset = true;
    
    // Restore the previous fire rate
    spaceship.SetFireRate(currentFireRate);
}

void Game::Draw()
{
    spaceship.Draw();
    mysteryShip.Draw();

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
    bool running = (paused == false && lostWindowFocus == false && isInExitMenu == false && gameOver == false && lostLife == false);
    if (running)
    {
        double currentTime = GetTime();
        {
            if (currentTime - timeLastMysteryShipSpawn > mysteryShipSpawnInterval)
            {
                mysteryShip.Spawn();
                timeLastMysteryShipSpawn = GetTime();
            }
        }
        HandleInput();

        spaceship.Update();
        mysteryShip.Update();

        MoveAliens(alienShipSpeed);
        AlienShootLaser();

        for (auto &alienLaser : alienLasers)
        {
            alienLaser.Update();
        }

        DeleteInactiveAlienLasers();

        CheckForCollisions();
        
        // Check if all aliens are defeated
        if (aliens.empty())
        {
            AdvanceLevel();
        }
    }
    else if (lostLife)
    {
        // Update debuff timer
        lostLifeTimer += GetFrameTime();
        
        // Only allow continuing after 300ms debuff
        if (lostLifeTimer >= 0.3f && GetKeyPressed() != 0)
        {
            lostLife = false;
            lostLifeTimer = 0.0f;  // Reset timer
            spaceship.Reset();  // Reset player position
        }
    }
    else if (gameOver)
    {
        // Check for space key to restart the game
        if (IsKeyPressed(KEY_SPACE))
        {
            gameOver = false;
            isFirstFrameAfterReset = true;  // Reset this flag when restarting
            InitGame();  // Reset the entire game state
        }
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
    if(isFirstFrameAfterReset)
    {
        isFirstFrameAfterReset = false;
        return;
    }

    // Movement controls (both arrow keys and WASD)
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
    {
        spaceship.MoveLeft();
    }
    else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
    {
        spaceship.MoveRight();
    }

    // Fire control (both space and W)
    if (IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_W))
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
        if (!laser.active) continue;

        // Check alien collisions first
        auto it = aliens.begin();
        while (it != aliens.end())
        {
            if (CheckCollisionRecs(it->getRect(), laser.getRect()))
            {
                PlaySound(explosionSound);

                if (it->type == 1)
                {
                    score += 100;
                }
                else if (it->type == 2)
                {
                    score += 200;
                }
                else if (it->type == 3)
                {
                    score += 300;
                }

                CheckForHighScore();
                it = aliens.erase(it);
                laser.active = false;
                break;
            }
            else
            {
                ++it;
            }
        }

        if (!laser.active) continue;

        // Check obstacle collisions
        for (auto &obs : obstacles)
        {
            auto it = obs.blocks.begin();
            while (it != obs.blocks.end())
            {
                if (CheckCollisionRecs(it->getRect(), laser.getRect()))
                {
                    it = obs.blocks.erase(it);
                    laser.active = false;
                    break;
                }
                else
                {
                    ++it;
                }
            }
            if (!laser.active) break;
        }

        if (!laser.active) continue;

        // Check mystery ship collision
        if (CheckCollisionRecs(mysteryShip.getRect(), laser.getRect()))
        {
            PlaySound(explosionSound);
            mysteryShip.alive = false;
            laser.active = false;
            score += 500;
            CheckForHighScore();
        }
    }

    // Alien Lasers
    for (auto &laser : alienLasers)
    {
        if (laser.active == false)
        {
            continue;
        }

        if (CheckCollisionRecs(laser.getRect(), spaceship.getRect()))
        {
            laser.active = false;
            lives--;
            PlaySound(explosionSound);
            lostLife = true;
            lostLifeTimer = 0.0f;  // Reset debuff timer when hit
            if (lives == 0)
            {
                GameOver();
            }
            break;
        }
        if (laser.active == false)
        {
            continue;
        }

        for (auto &obs : obstacles)
        {
            auto it = obs.blocks.begin();
            while (it != obs.blocks.end())
            {
                if (CheckCollisionRecs(it->getRect(), laser.getRect()))
                {
                    it = obs.blocks.erase(it);
                    laser.active = false;
                    break;
                }
                else
                {
                    ++it;
                }
            }
            if (laser.active == false)
            {
                break;
            }
        }

        if (laser.active == false)
        {
            continue;
        }
    }

    // alien collision with obstacle
    for (auto &alien : aliens)
    {
        for (auto &obstacle : obstacles)
        {
            auto it = obstacle.blocks.begin();
            while (it != obstacle.blocks.end())
            {
                if (CheckCollisionRecs(it->getRect(), alien.getRect()))
                {
                    it = obstacle.blocks.erase(it);
                }
                else
                {
                    it++;
                }
            }
        }

        if (CheckCollisionRecs(alien.getRect(), spaceship.getRect()))
        {
            GameOver();
        }
    }
}

void Game::GameOver()
{
    gameOver = true;
}

void Game::CheckForHighScore()
{
    if (score > highScore)
    {
        highScore = score;
        SaveHighScoreToFile();
    }
}

void Game::SaveHighScoreToFile()
{
    std::ofstream highScoreFile("highscore.txt");
    if (highScoreFile.is_open())
    {
        highScoreFile << highScore;
        highScoreFile.close();
    }
    else
    {
        std::cerr << "Failed to save highscore to file \n";
    }
}

int Game::LoadHighScoreFromFile()
{
    int loadedHighScore = 0;
    std::ifstream highscoreFile("highscore.txt");
    if (highscoreFile.is_open())
    {
        highscoreFile >> loadedHighScore;
        highscoreFile.close();
    }
    else
    {
        std::cerr << "Failed to load highscore from file\n";
    }
    return loadedHighScore;
}

void Game::CreateObstacles()
{
    int obstacleWidth = Obstacle::grid[0].size() * Block::blockWidth;
    float gap = (gameScreenWidth - (4 * obstacleWidth)) / 5;

    for (int i = 0; i < 4; i++)
    {
        float offsetX = (i + 1) * gap + i * obstacleWidth;
        obstacles.push_back(Obstacle({offsetX, float(gameScreenHeight - 200)}));
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

void Game::MoveAliens(float speed)
{
    if (alienUpdateTimerExpired)
    {
        for (auto &alien : aliens)
        {
            if (alien.position.x + alien.alienImages[alien.type - 1].width > gameScreenWidth - frameOffsetRight)
            {
                aliensDirection = -1;
                MoveDownAliens(4);
            }
            if (alien.position.x < frameOffsetLeft)
            {
                aliensDirection = 1;
                MoveDownAliens(4);
            }
            alien.Update(speed, aliensDirection);
        }
        alienUpdateTimerExpired = false;
    }
    else
    {
        alienUpdateTimer += GetFrameTime();
        if (alienUpdateTimer >= alienUpdateRate)
        {
            alienUpdateTimerExpired = true;
            alienUpdateTimer = 0.0f;
        }
    }
}

void Game::MoveDownAliens(int distance)
{
    for (auto &alien : aliens)
    {
        alien.position.y += distance;
    }
}

void Game::AdvanceLevel()
{
    currentLevel++;
    // Increase alien speed by 10% each level
    alienShipSpeed *= 1.1f;
    // Increase alien fire rate by 5% each level
    alienLaserSpeed *= 1.05f;
    
    // Update player's fire rate to scale with level
    float currentFireRate = spaceship.GetFireRate();
    float newFireRate = currentFireRate * 0.9f;  // Make fire rate 10% faster each level
    spaceship.SetFireRate(newFireRate);
    
    // Clear existing aliens and create new ones
    aliens.clear();
    CreateAliens();
    
    // Reset alien movement direction
    aliensDirection = 1;
}