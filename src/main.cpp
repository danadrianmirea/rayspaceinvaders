#include <raylib.h>
// #include "raymath.h"
#include "game.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

int main()
{
    Color grey = Color{29, 29, 27, 255};
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(gameScreenWidth, gameScreenHeight, "Space invaders");
    SetExitKey(KEY_NULL); // Disable KEY_ESCAPE to close window, X-button still works

    bool exitWindowRequested = false; // Flag to request window to exit
    bool exitWindow = false;          // Flag to set window to exit

    int display = GetCurrentMonitor();

    float resScale = 0.8f;

    int windowWidth = (int)(GetMonitorWidth(display) * resScale);
    int windowHeight = (int)(GetMonitorHeight(display) * resScale);
    SetWindowSize(windowWidth, windowHeight);
    // ToggleBorderlessWindowed();

    RenderTexture2D target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR); // Texture scale filter to use

    SetTargetFPS(144);

    Game game;

    // bool IsWindowFocused(void); // Check if window is currently focused (only PLATFORM_DESKTOP)

    while (!exitWindow)
    {
        float scale = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);

        if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE))
        {
            exitWindowRequested = true;
            game.paused = true;
        }

        if (IsKeyPressed(KEY_ENTER) && (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)))
        {
            ToggleBorderlessWindowed();
        }

        if (exitWindowRequested)
        {
            if (IsKeyPressed(KEY_Y))
            {
                exitWindow = true;
            }
            else if (IsKeyPressed(KEY_N))
            {
                exitWindowRequested = false;
                game.paused = false;
            }
        }

        game.HandleInput();
        game.Update();

        BeginTextureMode(target);

        ClearBackground(grey);
        game.Draw();
        EndTextureMode();

        BeginDrawing();
        ClearBackground(GREEN);

        DrawTexturePro(target.texture, (Rectangle){0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height},
                       (Rectangle){(GetScreenWidth() - ((float)gameScreenWidth * scale)) * 0.5f, (GetScreenHeight() - ((float)gameScreenHeight * scale)) * 0.5f,
                                   (float)gameScreenWidth * scale, (float)gameScreenHeight * scale},
                       (Vector2){0, 0}, 0.0f, WHITE);

        if (exitWindowRequested)
        {
            DrawRectangle(GetScreenWidth() / 2 - 500,  GetScreenHeight()/ 2 - 40, 1000, 120, BLACK);
            DrawText("Are you sure you want to exit? [Y/N]", GetScreenWidth() / 2 - 400, GetScreenHeight() / 2, 40, WHITE);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}