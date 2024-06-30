#include <raylib.h>
#include "spaceship.h"

int main()
{
    Color darkGreen = Color{20, 160, 133, 255};

    const int screenWidth = 800;
    const int screenHeight = 600;

    Spaceship spaceship;

    InitWindow(screenWidth, screenHeight, "Space invaders");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(darkGreen);
        spaceship.Update();
        spaceship.Draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}