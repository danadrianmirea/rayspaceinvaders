#include <raylib.h>
// #include "raymath.h"
#include "game.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))


bool exitWindowRequested = false; // Flag to request window to exit
bool exitWindow = false;          // Flag to set window to exit

void UpdateWindow(Game& game, float scale)
{
    if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE))
    {
        exitWindowRequested = true;
        game.paused = true;
    }

    if (IsKeyPressed(KEY_ENTER) && (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)))
    {
        // ToggleBorderlessWindowed();
        ToggleFullscreen();
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

    if (IsWindowFocused() == false)
    {
        game.paused = true;
    }
}

int main()
{
    Color grey = Color{29, 29, 27, 255};

    InitWindow(gameScreenWidth, gameScreenHeight, "Space invaders");
    SetExitKey(KEY_NULL); // Disable KEY_ESCAPE to close window, X-button still works

    // int display = GetCurrentMonitor();
    //  int windowWidth = (int)(GetMonitorWidth(display));
    //  int windowHeight = (int)(GetMonitorHeight(display));
    //  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

    int windowWidth = 1920;
    int windowHeight = 1080;
    SetWindowSize(windowWidth, windowHeight);
    ToggleFullscreen();
    // ToggleBorderlessWindowed();

    RenderTexture2D target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR); // Texture scale filter to use
    SetTargetFPS(144);

    float scale = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);

    Game game;

    while (!exitWindow)
    {
        scale = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);
        UpdateWindow(game, scale);

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
            DrawRectangle(GetScreenWidth() / 2 - 500, GetScreenHeight() / 2 - 40, 1000, 120, BLACK);
            DrawText("Are you sure you want to exit? [Y/N]", GetScreenWidth() / 2 - 400, GetScreenHeight() / 2, 40, WHITE);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}