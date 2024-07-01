#pragma once

#include "spaceship.h"
#include "obstacle.h"
#include "alien.h"

class Game
{
    public:
        Game();
         ~Game();
        Game(const Game&) = delete;
        const Game& operator=(const Game& g) = delete;
        Game(Game &&) = delete;
        Game&& operator=(Game && g) = delete;

        void Draw();
        void Update();
        void DeleteInactiveAlienLasers();
        void HandleInput();
        void AlienShootLaser();
        
    private:
        Spaceship spaceship;
        void CreateObstacles();
        void CreateAliens();
        void MoveAliens();
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

        bool aliensReadyToFire;
        float alienFireTimer;
        const float alienFireRate = 0.350f;

};