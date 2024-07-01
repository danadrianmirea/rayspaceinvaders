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
        void HandleInput();
        
    private:
        Spaceship spaceship;
        void CreateObstacles();
        void CreateAliens();
        std::vector<Obstacle> obstacles;
        std::vector<Alien> aliens;

        const int numAlienRows = 5;
        const int numAlienColumns = 11;
        const int alienCellSize = 55;
        const int alienOffsetX = 75;
        const int alienOffsetY = 100;

};