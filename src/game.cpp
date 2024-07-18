
#include "game.h"
#include <algorithm>
#include <fstream>

#include <iostream>

Game::Game()
{   
    music = LoadMusicStream("Sounds/music.ogg");
    explosionSound = LoadSound("Sounds/explosion.ogg");
    font = LoadFontEx("Font/monogram.ttf", 64, 0, 0);
    target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR); // Texture scale filter to use
    firstTimeGameStart = true;
    //PlayMusicStream(music);
    InitGame();
}

Game::~Game()
{
    Alien::UnloadImages();
    UnloadMusicStream(music);
    UnloadSound(explosionSound);
    UnloadFont(font);
    CloseAudioDevice();
    UnloadRenderTexture(target);
}

void Game::InitGame()
{
    CreateObstacles();
    CreateAliens();
    aliensDirection = 1;    
    aliensReadyToFire = false;
    alienFireTimer = 0.0f;
    paused = false;
    lostWindowFocus = false;
    isInExitMenu = false;
    alienUpdateTimer = 0.0f;
    alienUpdateTimerExpired = false;
    lives = 3;
    score = 0;
    highScore = LoadHighScoreFromFile();
    gameOver = false;
    timeLastMysteryShipSpawn = 0.0f;
    mysteryShipSpawnInterval = GetRandomValue(10, 20);
    isFirstFrameAfterReset = true;
    level = 1;
    screenScale = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);
}

void Game::Reset()
{
    obstacles.clear();
    aliens.clear();
    alienLasers.clear();
    spaceship.Reset();
    InitGame();
}

void Game::Draw()
{
    BeginTextureMode(target);
    ClearBackground(grey);
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

    DrawUI();
    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexturePro(target.texture, (Rectangle){0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height},
                   (Rectangle){(GetScreenWidth() - ((float)gameScreenWidth * screenScale)) * 0.5f, (GetScreenHeight() - ((float)gameScreenHeight * screenScale)) * 0.5f, (float)gameScreenWidth * screenScale, (float)gameScreenHeight * screenScale},
                   (Vector2){0, 0}, 0.0f, WHITE);

    DrawScreenSpaceUI();
    EndDrawing();
}

void Game::DrawUI()
{
    DrawRectangleRoundedLines({10, 10, 780, 780}, 0.18f, 20, 2, yellow);

    DrawLineEx({25, 730}, {775, 730}, 3, yellow);
    std::string levelText = "Level " + FormatWithLeadingZeroes(level, 2);
    DrawTextEx(font, levelText.c_str(), {570, 740}, 34, 2, yellow);

    DrawTextEx(font, "Score", {50, 15}, 34, 2, yellow);
    std::string scoreText = FormatWithLeadingZeroes(score, 7);
    DrawTextEx(font, scoreText.c_str(), {50, 40}, 34, 2, yellow);

    DrawTextEx(font, "High Score", {570, 15}, 34, 2, yellow);
    std::string highScoreText = FormatWithLeadingZeroes(highScore, 7);
    DrawTextEx(font, highScoreText.c_str(), {570, 40}, 34, 2, yellow);

    float x = 50.0f;
    for (int i = 0; i < lives; i++)
    {
        DrawTextureV(spaceship.GetSpaceshipImage(), {x, 745}, WHITE);
        x += 50;
    }
}

void Game::Update(float dt)
{
    //UpdateMusicStream(music);

    screenScale = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);
    UpdateUI();

    bool running = (firstTimeGameStart == false && paused == false && lostWindowFocus == false && isInExitMenu == false && gameOver == false);
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
        HandleInput(dt);

        spaceship.Update(dt);
        mysteryShip.Update(dt);

        MoveAliens(dt, alienShipSpeeds[level-1]);
        AlienShootLaser();

        for (auto &alienLaser : alienLasers)
        {
            alienLaser.Update(dt);
        }

        DeleteInactiveAlienLasers();

        CheckForCollisions();
    }
}

void Game::UpdateUI()
{
    if (WindowShouldClose() || (IsKeyPressed(KEY_ESCAPE) && exitWindowRequested == false))
    {
        exitWindowRequested = true;
        isInExitMenu = true;
        return;
    }

    if (firstTimeGameStart && IsKeyPressed(KEY_SPACE))
    {
        firstTimeGameStart = false;
    }
    else if (gameOver && IsKeyPressed(KEY_SPACE))
    {
        Reset();
    }
    
    if (exitWindowRequested)
    {
        if (IsKeyPressed(KEY_Y))
        {
            exitWindow = true;
        }
        else if (IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE))
        {
            exitWindowRequested = false;
            isInExitMenu = false;
        }
    }

    if (IsWindowFocused() == false)
    {
        lostWindowFocus = true;
    }
    else
    {
        lostWindowFocus = false;
    }

    if (exitWindowRequested == false && lostWindowFocus == false && gameOver == false && isFirstFrameAfterReset == false && IsKeyPressed(KEY_P))
    {
        if (paused)
        {
            paused = false;
        }
        else
        {
            paused = true;
        }
    }
}

void Game::DeleteInactiveAlienLasers()
{
    auto it = std::remove_if(alienLasers.begin(), alienLasers.end(), [](Laser &l)
                             { return l.active == false; });
    alienLasers.erase(it, alienLasers.end());
}

void Game::HandleInput(float dt)
{
    if (isFirstFrameAfterReset)
    {
        isFirstFrameAfterReset = false;
        return;
    }

    if (IsKeyDown(KEY_LEFT))
    {
        spaceship.MoveLeft(dt);
    }
    else if (IsKeyDown(KEY_RIGHT))
    {
        spaceship.MoveRight(dt);
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
            alienLasers.push_back(Laser({alien.position.x + alien.alienImages[alien.type - 1].width / 2, alien.position.y + alien.alienImages[alien.type - 1].height}, alienLaserSpeeds[level-1]));
            aliensReadyToFire = false;
            alienFireTimer = 0.0f;
        }
        else
        {
            alienFireTimer += GetFrameTime();
            if (alienFireTimer >= alienFireRates[level-1])
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
        if (laser.active == false)
        {
            continue;
        }

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

void Game::MoveAliens(float dt, int speed)
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
            alien.Update(dt, speed, aliensDirection);
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

std::string Game::FormatWithLeadingZeroes(int number, int width)
{
    std::string numberText = std::to_string(number);
    int leadingZeros = width - numberText.length();
    numberText = std::string(leadingZeros, '0') + numberText;
    return numberText;
}

void Game::DrawScreenSpaceUI()
{
    if (exitWindowRequested)
    {
        DrawRectangleRounded({(float)(GetScreenWidth() / 2 - 500), (float)(GetScreenHeight() / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
        DrawText("Are you sure you want to exit? [Y/N]", GetScreenWidth() / 2 - 400, GetScreenHeight() / 2, 40, yellow);
    }
    else if (firstTimeGameStart)
    {
        DrawRectangleRounded({(float)(GetScreenWidth() / 2 - 500), (float)(GetScreenHeight() / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
        DrawText("Press SPACE to play", GetScreenWidth() / 2 - 200, GetScreenHeight() / 2, 40, yellow);
    }

    else if (paused)
    {
        DrawRectangleRounded({(float)(GetScreenWidth() / 2 - 500), (float)(GetScreenHeight() / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
        DrawText("Game paused, press P to continue", GetScreenWidth() / 2 - 400, GetScreenHeight() / 2, 40, yellow);
    }
    else if (lostWindowFocus)
    {
        DrawRectangleRounded({(float)(GetScreenWidth() / 2 - 500), (float)(GetScreenHeight() / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
        DrawText("Game paused, focus window to continue", GetScreenWidth() / 2 - 400, GetScreenHeight() / 2, 40, yellow);
    }
    else if (gameOver)
    {
        DrawRectangleRounded({(float)(GetScreenWidth() / 2 - 500), (float)(GetScreenHeight() / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
        DrawText("Game over, press SPACE to play again", GetScreenWidth() / 2 - 400, GetScreenHeight() / 2, 40, yellow);
    }
}
