#include <raylib.h>
#include "spaceship.h"

int main()
{
    Color grey = Color { 29, 29, 27, 255 };

    const int screenWidth = 750;
    const int screenHeight = 700;

    InitWindow(screenWidth, screenHeight, "Space invaders");
    SetTargetFPS(144);

    Spaceship spaceship;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(grey);
        spaceship.Update();
        spaceship.Draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}