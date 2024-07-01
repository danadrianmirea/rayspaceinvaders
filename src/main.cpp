#include <raylib.h>
#include "game.h"

int main()
{
    Color grey = Color{29, 29, 27, 255};

    const int screenWidth = 750;
    const int screenHeight = 700;

    InitWindow(screenWidth, screenHeight, "Space invaders");
    SetTargetFPS(144);

    Game game;

    while (!WindowShouldClose())
    {
        game.HandleInput();
        game.Update();
        BeginDrawing();
        ClearBackground(grey);
        game.Draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}