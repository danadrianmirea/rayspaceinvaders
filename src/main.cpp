#include <string>
#include <raylib.h>
// #include "raymath.h"
#include "game.h"
#include "globals.h"

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

Color grey = Color{ 29, 29, 27, 255 };
Color yellow = Color{ 243, 216, 63, 255 };

int offset = 50;

// Global variables for the game loop
Game* gameInstance = nullptr;
Font gameFont;
RenderTexture2D gameTarget;
float gameScale = 1.0f;
Texture2D spaceshipImage;

void UpdateWindow(Game& game, float scale)
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

    if (game.gameOver && IsKeyPressed(KEY_SPACE))
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

// Main game loop function that will be called by emscripten_set_main_loop
void GameLoop()
{
#if MUSIC == 1
    UpdateMusicStream(gameInstance->music);
#endif

    gameScale = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);
    UpdateWindow(*gameInstance, gameScale);

    gameInstance->Update();

    BeginTextureMode(gameTarget);

    ClearBackground(grey);
    Rectangle border = { 10, 10, 780, 780 };
    DrawRectangleRoundedLinesEx(border, 0.18f, 20, 2.0f, yellow);

    DrawLineEx({ 25, 730 }, { 775, 730 }, 3, yellow);
    std::string levelText = "LEVEL " + FormatWithLeadingZeroes(gameInstance->currentLevel, 2);
    DrawTextEx(gameFont, levelText.c_str(), { 570, 740 }, 34, 2, yellow);

    DrawTextEx(gameFont, "SCORE", { 50, 15 }, 34, 2, yellow);
    std::string scoreText = FormatWithLeadingZeroes(gameInstance->score, 7);
    DrawTextEx(gameFont, scoreText.c_str(), { 50, 40 }, 34, 2, yellow);

    DrawTextEx(gameFont, "HIGH-SCORE", { 570, 15 }, 34, 2, yellow);
    std::string highScoreText = FormatWithLeadingZeroes(gameInstance->highScore, 7);
    DrawTextEx(gameFont, highScoreText.c_str(), { 570, 40 }, 34, 2, yellow);

    float x = 50.0f;
    for (int i = 0; i < gameInstance->lives; i++)
    {
        DrawTextureV(spaceshipImage, { x, 745 }, WHITE);
        x += 50;
    }

    gameInstance->Draw();
    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);

    DrawTexturePro(gameTarget.texture, (Rectangle) { 0.0f, 0.0f, (float)gameTarget.texture.width, (float)-gameTarget.texture.height },
        (Rectangle) {
        (GetScreenWidth() - ((float)gameScreenWidth * gameScale)) * 0.5f, (GetScreenHeight() - ((float)gameScreenHeight * gameScale)) * 0.5f,
            (float)gameScreenWidth* gameScale, (float)gameScreenHeight* gameScale
    },
        (Vector2) {
        0, 0
    }, 0.0f, WHITE);

    if (exitWindowRequested)
    {
        DrawRectangleRounded({ (float)(GetScreenWidth() / 2 - 500), (float)(GetScreenHeight() / 2 - 40), 1000, 120 }, 0.76f, 20, BLACK);
        DrawText("Are you sure you want to exit? [Y/N]", GetScreenWidth() / 2 - 400, GetScreenHeight() / 2, 40, yellow);
    }
    else if (gameInstance->paused)
    {
        DrawRectangleRounded({ (float)(GetScreenWidth() / 2 - 500), (float)(GetScreenHeight() / 2 - 40), 1000, 120 }, 0.76f, 20, BLACK);
        DrawText("Game paused, press P to continue", GetScreenWidth() / 2 - 400, GetScreenHeight() / 2, 40, yellow);
    }
    else if (gameInstance->lostWindowFocus)
    {
        DrawRectangleRounded({ (float)(GetScreenWidth() / 2 - 500), (float)(GetScreenHeight() / 2 - 40), 1000, 120 }, 0.76f, 20, BLACK);
        DrawText("Game paused, focus window to continue", GetScreenWidth() / 2 - 400, GetScreenHeight() / 2, 40, yellow);
    }
    else if (gameInstance->gameOver)
    {
        DrawRectangleRounded({ (float)(GetScreenWidth() / 2 - 500), (float)(GetScreenHeight() / 2 - 40), 1000, 120 }, 0.76f, 20, BLACK);
        DrawText("Game over, press SPACE to play again", GetScreenWidth() / 2 - 400, GetScreenHeight() / 2, 40, yellow);
    }
    else if (gameInstance->lostLife)
    {
        DrawRectangleRounded({ (float)(GetScreenWidth() / 2 - 500), (float)(GetScreenHeight() / 2 - 40), 1000, 120 }, 0.76f, 20, BLACK);
        DrawText("You lost a life! Press any key to continue", GetScreenWidth() / 2 - 400, GetScreenHeight() / 2, 40, yellow);
    }

    EndDrawing();
}

int main()
{
    InitWindow(gameScreenWidth, gameScreenHeight, "Space invaders");
    InitAudioDevice();
    SetMasterVolume(0.22f);
    SetExitKey(KEY_NULL); // Disable KEY_ESCAPE to close window, X-button still works

    gameFont = LoadFontEx("Font/monogram.ttf", 64, 0, 0);

    //  int display = GetCurrentMonitor();
    //  int windowWidth = (int)(GetMonitorWidth(display));
    //  int windowHeight = (int)(GetMonitorHeight(display));
    //  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);.

    SetWindowSize(windowWidth - borderW, windowHeight - borderH);
    // MaximizeWindow();
    SetWindowPosition(50, 50);
    // ToggleFullscreen();
    ToggleBorderlessWindowed();

    gameScreenWidth = gameScreenWidth + offset;
    gameScreenHeight = gameScreenHeight + 2 * offset;
    gameTarget = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(gameTarget.texture, TEXTURE_FILTER_BILINEAR); // Texture scale filter to use
    SetTargetFPS(144);

    gameScale = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);

    gameInstance = new Game();
    spaceshipImage = LoadTexture("Graphics/spaceship.png");

#ifdef EMSCRIPTEN_BUILD
    // Use emscripten_set_main_loop for Emscripten builds
    emscripten_set_main_loop(GameLoop, 0, 1);
#else
    // Regular desktop game loop
    while (!exitWindow)
    {
        GameLoop();
    }
#endif

    // Cleanup
    delete gameInstance;
    CloseWindow();
    UnloadFont(gameFont);
    UnloadTexture(spaceshipImage);
    UnloadRenderTexture(gameTarget);
    CloseAudioDevice();
    return 0;
}