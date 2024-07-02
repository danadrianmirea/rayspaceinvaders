#include <string>
#include <raylib.h>
// #include "raymath.h"
#include "game.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

bool exitWindowRequested = false; // Flag to request window to exit
bool exitWindow = false;          // Flag to set window to exit
bool fullscreen = true;
const int borderH = 100;
const int borderW = (int)(1920.0f / 1080.f * borderH);

std::string FormatWithLeadingZeroes(int number, int width)
{
    std::string numberText = std::to_string(number);
    int leadingZeros = width - numberText.length();
    numberText = std::string(leadingZeros, '0') + numberText;
    return numberText;
}

int windowWidth = 1920;
int windowHeight = 1080;

Color grey = Color{29, 29, 27, 255};
Color yellow = Color{243, 216, 63, 255};

int offset = 50;

void UpdateWindow(Game &game, float scale)
{
    if (WindowShouldClose() || (IsKeyPressed(KEY_ESCAPE) && exitWindowRequested == false))
    {
        exitWindowRequested = true;
        game.isInExitMenu = true;
        return;
    }

    if (IsKeyPressed(KEY_ENTER) && (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)))
    {
        if (fullscreen)
        {
            fullscreen = false;
            SetWindowSize(windowWidth - borderW, windowHeight - borderH);
        }
        else
        {
            fullscreen = true;
            SetWindowSize(windowWidth, windowHeight);
        }
        // ToggleFullscreen();
        ToggleBorderlessWindowed();
    }

    if (game.gameOver && IsKeyPressed(KEY_ENTER))
    {
        game.Reset();
    }

    if (exitWindowRequested)
    {
        if (IsKeyPressed(KEY_Y))
        {
            exitWindow = true;
        }
        else if (IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE))
        {
            exitWindowRequested = false;
            game.isInExitMenu = false;
        }
    }

    if (IsWindowFocused() == false)
    {
        game.lostWindowFocus = true;
    }
    else
    {
        game.lostWindowFocus = false;
    }

    if (exitWindowRequested == false && game.lostWindowFocus == false && game.gameOver == false && IsKeyPressed(KEY_P))
    {
        if (game.paused)
        {
            game.paused = false;
        }
        else
        {
            game.paused = true;
        }
    }
}

int main()
{
    InitWindow(gameScreenWidth, gameScreenHeight, "Space invaders");
    InitAudioDevice();
    SetMasterVolume(0.22f);
    SetExitKey(KEY_NULL); // Disable KEY_ESCAPE to close window, X-button still works

    Font font = LoadFontEx("Font/monogram.ttf", 64, 0, 0);

    //  int display = GetCurrentMonitor();
    //  int windowWidth = (int)(GetMonitorWidth(display));
    //  int windowHeight = (int)(GetMonitorHeight(display));
    //  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);.

    SetWindowSize(windowWidth - borderW, windowHeight - borderH);
    // MaximizeWindow();
    SetWindowPosition(50, 50);
    // ToggleFullscreen();
    // ToggleBorderlessWindowed();

    gameScreenWidth = gameScreenWidth + offset;
    gameScreenHeight = gameScreenHeight + 2 * offset;
    RenderTexture2D target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR); // Texture scale filter to use
    SetTargetFPS(144);

    float scale = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);

    Game game;

    while (!exitWindow)
    {
        UpdateMusicStream(game.music);
        
        scale = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);
        UpdateWindow(game, scale);

        game.Update();

        BeginTextureMode(target);

        ClearBackground(grey);
        DrawRectangleRoundedLines({10, 10, 780, 780}, 0.18f, 20, 2, yellow);

        DrawLineEx({25, 730}, {775, 730}, 3, yellow);
        DrawTextEx(font, "LEVEL 01", {570, 740}, 34, 2, yellow);

        DrawTextEx(font, "SCORE", {50, 15}, 34, 2, yellow);
        std::string scoreText = FormatWithLeadingZeroes(game.score, 7);
        DrawTextEx(font, scoreText.c_str(), {50, 40}, 34, 2, yellow);

        DrawTextEx(font, "HIGH-SCORE", {570, 15}, 34, 2, yellow);
        std::string highScoreText = FormatWithLeadingZeroes(game.highScore, 7);
        DrawTextEx(font, highScoreText.c_str(), {570, 40}, 34, 2, yellow);

        Texture2D spaceshipImage = LoadTexture("Graphics/spaceship.png");

        float x = 50.0f;
        for (int i = 0; i < game.lives; i++)
        {
            DrawTextureV(spaceshipImage, {x, 745}, WHITE);
            x += 50;
        }

        game.Draw();
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexturePro(target.texture, (Rectangle){0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height},
                       (Rectangle){(GetScreenWidth() - ((float)gameScreenWidth * scale)) * 0.5f, (GetScreenHeight() - ((float)gameScreenHeight * scale)) * 0.5f,
                                   (float)gameScreenWidth * scale, (float)gameScreenHeight * scale},
                       (Vector2){0, 0}, 0.0f, WHITE);

        if (exitWindowRequested)
        {
            DrawRectangleRounded({(float)(GetScreenWidth() / 2 - 500), (float)(GetScreenHeight() / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
            DrawText("Are you sure you want to exit? [Y/N]", GetScreenWidth() / 2 - 400, GetScreenHeight() / 2, 40, yellow);
        }
        else if (game.paused)
        {
            DrawRectangleRounded({(float)(GetScreenWidth() / 2 - 500), (float)(GetScreenHeight() / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
            DrawText("Game paused, press P to continue", GetScreenWidth() / 2 - 400, GetScreenHeight() / 2, 40, yellow);
        }
        else if (game.lostWindowFocus)
        {
            DrawRectangleRounded({(float)(GetScreenWidth() / 2 - 500), (float)(GetScreenHeight() / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
            DrawText("Game paused, focus window to continue", GetScreenWidth() / 2 - 400, GetScreenHeight() / 2, 40, yellow);
        }
        else if (game.gameOver)
        {
            DrawRectangleRounded({(float)(GetScreenWidth() / 2 - 500), (float)(GetScreenHeight() / 2 - 40), 1000, 120}, 0.76f, 20, BLACK);
            DrawText("Game over, press ENTER to play again", GetScreenWidth() / 2 - 400, GetScreenHeight() / 2, 40, yellow);
        }

        EndDrawing();
    }

    CloseWindow();
    UnloadFont(font);
    CloseAudioDevice();
    return 0;
}