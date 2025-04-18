#pragma once

#include "globals.h"
#include "spaceship.h"
#include "obstacle.h"
#include "alien.h"
#include "misteryship.h"

class Game
{
public:
    Game();
    ~Game();
    void InitGame();
    void Reset();
    Game(const Game &) = delete;
    const Game &operator=(const Game &g) = delete;
    Game(Game &&) = delete;
    Game &&operator=(Game &&g) = delete;

    void Draw();
    void Update();
    void DeleteInactiveAlienLasers();
    void HandleInput();
    void AlienShootLaser();
    void CheckForCollisions();
    void GameOver();
    void CheckForHighScore();
    void SaveHighScoreToFile();
    int LoadHighScoreFromFile();

    bool isInExitMenu;
    bool paused;
    bool lostWindowFocus;
    bool gameOver;
    bool lostLife;
    bool isFirstStartup;  // New state variable for first startup
    bool isFirstFrameAfterReset;  // Flag to ignore first input after state changes
    float lostLifeTimer;  // Timer for debuff after losing a life
    float startupDelayTimer;  // Timer for startup delay
    int lives;
    int score;
    int highScore;
    int currentLevel;  // Track current level
    Music music;
    Sound explosionSound;
    
private:
    Spaceship spaceship;
    void CreateObstacles();
    void CreateAliens();
    void MoveAliens(float speed);
    void MoveDownAliens(int distance);
    void AdvanceLevel();  // Function to handle level progression
    std::vector<Obstacle> obstacles;
    std::vector<Alien> aliens;
    std::vector<Laser> alienLasers;

    int aliensDirection;
    const int numAlienRows = 5;
    const int numAlienColumns = 11;
    const int alienCellSize = 55;
    const int alienOffsetX = 75;
    const int alienOffsetY = 100;

    float alienLaserSpeed;
    float alienShipSpeed;
    bool aliensReadyToFire;
    float alienFireTimer;
    const float alienFireRate = 0.350f;
    bool alienUpdateTimerExpired;
    float alienUpdateTimer;
    const float alienUpdateRate = 0.01f;

    MysteryShip mysteryShip;
    float mysteryShipSpawnInterval;
    float timeLastMysteryShipSpawn;
};