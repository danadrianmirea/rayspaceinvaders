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
    int lives;
    int score;
    int highScore;
    Music music;
    Sound explosionSound;
    
private:
    Spaceship spaceship;
    void CreateObstacles();
    void CreateAliens();
    void MoveAliens(int speed);
    void MoveDownAliens(int distance);
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
    bool isFirstFrameAfterReset;
    float alienUpdateTimer;
    const float alienUpdateRate = 0.01f;

    MysteryShip mysteryShip;
    float mysteryShipSpawnInterval;
    float timeLastMysteryShipSpawn;
};