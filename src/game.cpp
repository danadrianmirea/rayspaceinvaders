#include "game.h"
#include <algorithm>
#include <fstream>

#include <iostream>

#ifdef EMSCRIPTEN_BUILD
#include <emscripten.h>
#endif

// Initialize static variable
bool Game::isMobile = false;

// Implement the mobile detection method
void Game::DetectMobileDevice()
{
#ifdef EMSCRIPTEN_BUILD
    // Use Emscripten's JavaScript evaluation to check if the device is mobile
    EM_ASM(
        var isMobileDevice = /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent);
        // Store the result in Module so we can access it from C++
        Module.isMobileDevice = isMobileDevice;
    );
    
    // Get the result from JavaScript
    isMobile = EM_ASM_INT({
        return Module.isMobileDevice ? 1 : 0;
    });
    
    std::cout << "Running on " << (isMobile ? "mobile device" : "desktop browser") << std::endl;
#else
    isMobile = false;
#endif
}

#ifdef EMSCRIPTEN_BUILD
bool Game::IsPointInCircle(Vector2 point, Vector2 center, float radius)
{
    float dx = point.x - center.x;
    float dy = point.y - center.y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

bool Game::IsPointInRectangle(Vector2 point, Rectangle rect)
{
    return (point.x >= rect.x && point.x <= (rect.x + rect.width) &&
            point.y >= rect.y && point.y <= (rect.y + rect.height));
}

void Game::DrawMobileControls()
{
    if (!isMobile || paused || lostWindowFocus || isInExitMenu || gameOver || lostLife || isFirstStartup)
        return;
    
    // Draw fire button (red circle)
    DrawCircleV(fireButtonPos, fireButtonRadius, 
                fireButtonPressed ? RED : YELLOW);
    DrawCircleLines(fireButtonPos.x, fireButtonPos.y, fireButtonRadius, BLACK);
    
    // Draw laser indicator inside fire button
    float laserWidth = 16.0f;
    float laserHeight = fireButtonRadius * 0.8f;
    Rectangle laserRect = {
        fireButtonPos.x - laserWidth/2,
        fireButtonPos.y - laserHeight/2,
        laserWidth,
        laserHeight
    };
    DrawRectangleRec(laserRect, BLACK);
    
    // Draw movement buttons (rectangles)
    DrawRectangleRec(leftButtonRect, leftButtonPressed ? RED : YELLOW);
    DrawRectangleLinesEx(leftButtonRect, 2, BLACK);
    
    // Draw left arrow triangle (pointing left)
    DrawTriangle(
        (Vector2){leftButtonRect.x + leftButtonRect.width * 0.3f, leftButtonRect.y + leftButtonRect.height * 0.5f},  // Point (left)
        (Vector2){leftButtonRect.x + leftButtonRect.width * 0.7f, leftButtonRect.y + leftButtonRect.height * 0.7f},  // Bottom
        (Vector2){leftButtonRect.x + leftButtonRect.width * 0.7f, leftButtonRect.y + leftButtonRect.height * 0.3f},  // Top        
        BLACK
    );
    
    DrawRectangleRec(rightButtonRect, rightButtonPressed ? RED : YELLOW);
    DrawRectangleLinesEx(rightButtonRect, 2, BLACK);
    
    // Draw right arrow triangle (pointing right)
    DrawTriangle(
        (Vector2){rightButtonRect.x + rightButtonRect.width * 0.7f, rightButtonRect.y + rightButtonRect.height * 0.5f},  // Point (right)
        (Vector2){rightButtonRect.x + rightButtonRect.width * 0.3f, rightButtonRect.y + rightButtonRect.height * 0.3f},  // Top
        (Vector2){rightButtonRect.x + rightButtonRect.width * 0.3f, rightButtonRect.y + rightButtonRect.height * 0.7f},  // Bottom
        BLACK
    );
}

void Game::HandleMobileControls()
{
    if (!isMobile) return;
    
    // Reset button states
    leftButtonPressed = false;
    rightButtonPressed = false;
    fireButtonPressed = false;
    
    // Update debounce timer
    if (pauseDebounceTimer > 0.0f)
    {
        pauseDebounceTimer -= GetFrameTime();
        return;  // Skip input processing while debouncing
    }
    
    // Check for touches
    if (GetTouchPointCount() > 0)
    {
        for (int i = 0; i < GetTouchPointCount(); i++)
        {
            // Get touch position in screen space
            Vector2 touchPos = GetTouchPosition(i);
            float collisionRadius = 50.0f;
            float extraHeight = 100;            
            
            // Convert touch position from screen space to game screen space
            // First, subtract the game screen offset to get relative position
            float gameScreenX = (GetScreenWidth() - gameScreenWidth * gameScale) * 0.5f;
            float gameScreenY = (GetScreenHeight() - gameScreenHeight * gameScale) * 0.5f;
            
            // Then convert to game screen coordinates
            Vector2 gameSpaceTouchPos = {
                (touchPos.x - gameScreenX) / gameScale,
                (touchPos.y - gameScreenY) / gameScale
            };

            // Check for center screen tap to pause/unpause
            float centerX = gameScreenWidth / 2;
            float centerY = gameScreenHeight / 2;
            float centerRadius = 100.0f; // Radius for center tap detection
            
            if (IsPointInCircle(gameSpaceTouchPos, (Vector2){centerX, centerY}, centerRadius))
            {
                // Toggle pause state and set debounce timer
                paused = !paused;
                pauseDebounceTimer = 0.3f;  // 300ms debounce
                continue; // Skip other button checks if we're pausing
            }

            // Create a collision rectangle centered on the fire button
            Rectangle scaledFireButtonRect = {
                fireButtonPos.x - collisionRadius*3,
                fireButtonPos.y - collisionRadius*3 - extraHeight,
                collisionRadius * 6,
                collisionRadius * 6 + extraHeight
            };

            // Check fire button
            if (IsPointInRectangle(gameSpaceTouchPos, scaledFireButtonRect))
            {
                fireButtonPressed = true;
                spaceship.FireLaser();
            }
      
            Rectangle scaledLeftButtonRect = {
                leftButtonRect.x - collisionRadius,
                leftButtonRect.y - collisionRadius - extraHeight,
                leftButtonRect.width + 2 * collisionRadius,
                leftButtonRect.height + 2 * collisionRadius + extraHeight
            };
              
            Rectangle scaledRightButtonRect = {
                rightButtonRect.x - collisionRadius,
                rightButtonRect.y - collisionRadius - extraHeight,
                rightButtonRect.width + 2 * collisionRadius,
                rightButtonRect.height + 2 * collisionRadius + extraHeight
            };

            // Check movement buttons
            if (IsPointInRectangle(gameSpaceTouchPos, scaledLeftButtonRect))
            {
                leftButtonPressed = true;
                spaceship.MoveLeft();
            }

            if (IsPointInRectangle(gameSpaceTouchPos, scaledRightButtonRect))
            {
                rightButtonPressed = true;
                spaceship.MoveRight();
            }
        }
    }
}
#endif

Game::Game()
{
    explosionSound = LoadSound("Sounds/explosion.ogg");
#if MUSIC == 1
    music = LoadMusicStream("Sounds/music.ogg");
    PlayMusicStream(music);
#endif
    // Detect if we're on a mobile device
    DetectMobileDevice();

    InitGame();
    isFirstStartup = true;  // Initialize first startup state
    startupDelayTimer = 0.0f;  // Initialize startup delay timer
    pauseDebounceTimer = 0.0f;  // Initialize pause debounce timer
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
    isFirstFrameAfterReset = true;  // Set flag to ignore first input

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
    gameOver = false;  // Reset the game over flag
    pauseDebounceTimer = 0.0f;  // Reset pause debounce timer
    
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

#ifdef EMSCRIPTEN_BUILD
    DrawMobileControls();
#endif
}

void Game::Update()
{
    // Always handle input, even when paused
    HandleInput();

    bool running = (paused == false && lostWindowFocus == false && isInExitMenu == false && gameOver == false && lostLife == false && isFirstStartup == false);
    if (running)
    {
        // Handle startup delay
        if (startupDelayTimer > 0.0f)
        {
            startupDelayTimer -= GetFrameTime();
            if (startupDelayTimer <= 0.0f)
            {
                InitGame();  // Initialize game after delay expires
            }
            return;  // Skip game logic during startup delay
        }

        double currentTime = GetTime();
        {
            if (currentTime - timeLastMysteryShipSpawn > mysteryShipSpawnInterval)
            {
                mysteryShip.Spawn();
                timeLastMysteryShipSpawn = GetTime();
            }
        }

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
        
        // Only allow continuing after inputDelayTime
#ifdef EMSCRIPTEN_BUILD
        if (lostLifeTimer >= inputDelayTime && (GetKeyPressed() != 0 || (isMobile && IsGestureDetected(GESTURE_TAP))))
#else
        if (lostLifeTimer >= inputDelayTime && GetKeyPressed() != 0)
#endif
        {
            lostLife = false;
            lostLifeTimer = 0.0f;  // Reset timer
            spaceship.Reset();  // Reset player position
            spaceship.lasers.clear();  // Clear player lasers
            alienLasers.clear();  // Clear alien lasers
        }
    }
    else if (gameOver)
    {
        // Update game over delay timer
        gameOverTimer += GetFrameTime();
        
        // Only allow restarting after inputDelayTime
#ifdef EMSCRIPTEN_BUILD
        if (gameOverTimer >= inputDelayTime && (GetKeyPressed() != 0 || (isMobile && IsGestureDetected(GESTURE_TAP))))
#else
        if (gameOverTimer >= inputDelayTime && GetKeyPressed() != 0)
#endif
        {
            Reset();
            gameOver = false;
            gameOverTimer = 0.0f;  // Reset timer
            startupDelayTimer = 0.1f;  // Set a short delay before input is processed again
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
    // Clear the first frame flag if it's set
    if (isFirstFrameAfterReset)
    {
        isFirstFrameAfterReset = false;
        return;  // Skip input on the first frame
    }
    
    // Don't process input if game is not running
    if (lostWindowFocus || isInExitMenu || gameOver || lostLife || isFirstStartup)
    {
        return;
    }

#ifdef EMSCRIPTEN_BUILD
    if (isMobile)
    {
        // Handle mobile controls
        HandleMobileControls();
    }
    else
#endif
    {
        // Regular desktop/browser controls
        if (!paused)  // Only process these controls when not paused
        {
            // Movement controls (both arrow keys and WASD)
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
            {
                spaceship.MoveLeft();
            }
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
            {
                spaceship.MoveRight();
            }
            
            // Fire control (both space and W)
            if (IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_W))
            {
                spaceship.FireLaser();
            }
        }
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
    gameOverTimer = 0.0f;  // Initialize the game over delay timer
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
        obstacles.push_back(Obstacle({offsetX, float(gameScreenHeight - 250)}));
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