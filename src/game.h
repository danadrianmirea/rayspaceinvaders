#pragma once

#include <string>

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
    void DrawUI();
    void Update(float dt);
    void UpdateUI();
    void DeleteInactiveAlienLasers();
    void HandleInput();
    void AlienShootLaser();
    void CheckForCollisions();
    void GameOver();
    void CheckForHighScore();
    void SaveHighScoreToFile();
    int LoadHighScoreFromFile();

    bool firstTimeGameStart;
    bool isInExitMenu;
    bool paused;
    bool lostWindowFocus;
    bool gameOver;
    int lives;
    int level;
    int score;
    int highScore;
    Music music;
    Sound explosionSound;
    const int maxLevels = 6;

private:
    Spaceship spaceship;
    void CreateObstacles();
    void CreateAliens();
    void MoveAliens(float dt, int speed);
    void MoveDownAliens(int distance);
    void DrawScreenSpaceUI();
    std::string FormatWithLeadingZeroes(int number, int width);

    std::vector<Obstacle> obstacles;
    std::vector<Alien> aliens;
    std::vector<Laser> alienLasers;

    int aliensDirection;
    const int numAlienRows = 5;
    const int numAlienColumns = 11;
    const int alienCellSize = 55;
    const int alienOffsetX = 75;
    const int alienOffsetY = 100;

    static const int maxLevel = 10;
    float alienLaserSpeeds[maxLevel] = {-400, -400, -400, -400, -400, -400, -400, -400, -400, -400};
    float alienShipSpeeds[maxLevel] = {50, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    float alienFireRates[maxLevel] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

    bool aliensReadyToFire;
    float alienFireTimer;

    bool alienUpdateTimerExpired;
    bool isFirstFrameAfterReset;
    float alienUpdateTimer;

    const float alienUpdateRate = 0.01f;

    MysteryShip mysteryShip;
    float mysteryShipSpawnInterval;
    float timeLastMysteryShipSpawn;

    float screenScale;
    RenderTexture2D target;
    Font font;
};