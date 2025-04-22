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

#define WEB_CANVAS_WIDTH 960
#define WEB_CANVAS_HEIGHT 540

// Wrapper functions for screen dimensions are no longer needed as we use GetScreenWidth directly

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

void UpdateWindow(Game& game)
{
#ifndef EMSCRIPTEN_BUILD
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
#endif

#ifdef EMSCRIPTEN_BUILD
    if (game.gameOver && (IsKeyPressed(KEY_SPACE) || (Game::isMobile && IsGestureDetected(GESTURE_TAP))))
#else
    if (game.gameOver && IsKeyPressed(KEY_SPACE))
#endif
    {
        game.Reset();
        game.gameOver = false;  // Explicitly set gameOver to false
        game.startupDelayTimer = 0.1f;  // Set a short delay before input is processed again
    }

#ifndef EMSCRIPTEN_BUILD
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
 #endif

    if (IsWindowFocused() == false)
    {
        game.lostWindowFocus = true;
    }
    else
    {
        game.lostWindowFocus = false;
    }

#ifndef EMSCRIPTEN_BUILD
    if (exitWindowRequested == false && game.lostWindowFocus == false && game.gameOver == false && IsKeyPressed(KEY_P))    
#else
    if (exitWindowRequested == false && game.lostWindowFocus == false && game.gameOver == false && ( IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE) ) )
#endif
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

    UpdateWindow(*gameInstance);

#ifdef EMSCRIPTEN_BUILD
    // Update mobile control positions when screen size changes
    if (Game::isMobile)
    {
        const float buttonWidth = 80.0f;
        const float buttonHeight = 80.0f;
        const float buttonMargin = 100.0f;
        const int controlsY = gameScreenHeight;
        const float buttonY = controlsY - buttonHeight - 30.0f;

        // Fire button on left side (red circle)
        gameInstance->fireButtonRadius = 40.0f;
        gameInstance->fireButtonPos = (Vector2){ 
            gameInstance->fireButtonRadius + buttonMargin, 
            controlsY - gameInstance->fireButtonRadius - 30.0f
        };
        
        // Calculate positions based on screen width to ensure they're visible
        float rightSideX = gameScreenWidth - 2*buttonWidth - buttonMargin - buttonMargin;
        gameInstance->leftButtonRect = (Rectangle){ rightSideX, buttonY, buttonWidth, buttonHeight };
        gameInstance->rightButtonRect = (Rectangle){ rightSideX + buttonWidth + buttonMargin, buttonY, buttonWidth, buttonHeight };
    }
#endif

    gameInstance->Update();

    BeginTextureMode(gameTarget);

    ClearBackground(grey);
    Rectangle border = { 10, 10, 780, 780 };
    DrawRectangleRoundedLinesEx(border, 0.18f, 20, 2.0f * gameScale, yellow);

    DrawLineEx({ 25, 730 - 50 }, { 775, 730 - 50 }, 3 * gameScale, yellow);

    DrawTextEx(gameFont, "SCORE", { 50, 15 }, 34 * gameScale, 2 * gameScale, yellow);
    std::string scoreText = FormatWithLeadingZeroes(gameInstance->score, 7);
    DrawTextEx(gameFont, scoreText.c_str(), { 50, 40 }, 34 * gameScale, 2 * gameScale, yellow);

    DrawTextEx(gameFont, "HIGH-SCORE", { 570, 15 }, 34 * gameScale, 2 * gameScale, yellow);
    std::string highScoreText = FormatWithLeadingZeroes(gameInstance->highScore, 7);
    DrawTextEx(gameFont, highScoreText.c_str(), { 570, 40 }, 34 * gameScale, 2 * gameScale, yellow);

    float x = (float)gameScreenWidth/2 - 120;

    std::string levelText = "LEVEL " + FormatWithLeadingZeroes(gameInstance->currentLevel, 2);
    DrawTextEx(gameFont, levelText.c_str(), { x, 740 }, 34 * gameScale, 2 * gameScale, yellow);

    for (int i = 0; i < gameInstance->lives; i++)
    {
        DrawTextureEx(spaceshipImage, { x, 745 - 50 }, 0.0f, gameScale, WHITE);
        x += 50 * gameScale;
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
        DrawRectangleRounded({ (float)(GetScreenWidth() / 2 - 500 * gameScale), (float)(GetScreenHeight() / 2 - 40 * gameScale), 1000 * gameScale, 120 * gameScale }, 0.76f, 20 * gameScale, BLACK);
        DrawText("Are you sure you want to exit? [Y/N]", GetScreenWidth() / 2 - 400 * gameScale, GetScreenHeight() / 2, 40 * gameScale, yellow);
    }
    else if (gameInstance->paused)
    {
        DrawRectangleRounded({ (float)(GetScreenWidth() / 2 - 500 * gameScale), (float)(GetScreenHeight() / 2 - 40 * gameScale), 1000 * gameScale, 120 * gameScale }, 0.76f, 20 * gameScale, BLACK);
#ifdef EMSCRIPTEN_BUILD
        DrawText("Game paused, press P or ESC to continue", GetScreenWidth() / 2 - 400 * gameScale, GetScreenHeight() / 2, 40 * gameScale, yellow);
#else
        DrawText("Game paused, press P to continue", GetScreenWidth() / 2 - 400 * gameScale, GetScreenHeight() / 2, 40 * gameScale, yellow);
#endif
    }
    else if (gameInstance->lostWindowFocus)
    {
        DrawRectangleRounded({ (float)(GetScreenWidth() / 2 - 500 * gameScale), (float)(GetScreenHeight() / 2 - 40 * gameScale), 1000 * gameScale, 120 * gameScale }, 0.76f, 20 * gameScale, BLACK);
        DrawText("Game paused, focus window to continue", GetScreenWidth() / 2 - 400 * gameScale, GetScreenHeight() / 2, 40 * gameScale, yellow);
    }
    else if (gameInstance->gameOver)
    {
        DrawRectangleRounded({ (float)(GetScreenWidth() / 2 - 500 * gameScale), (float)(GetScreenHeight() / 2 - 40 * gameScale), 1000 * gameScale, 120 * gameScale }, 0.76f, 20 * gameScale, BLACK);
#ifdef EMSCRIPTEN_BUILD
        if (Game::isMobile) {
            DrawText("Game over, tap to play again", GetScreenWidth() / 2 - 400 * gameScale, GetScreenHeight() / 2, 40 * gameScale, yellow);
        } else {
#endif
            DrawText("Game over, press any key to play again", GetScreenWidth() / 2 - 400 * gameScale, GetScreenHeight() / 2, 40 * gameScale, yellow);
#ifdef EMSCRIPTEN_BUILD
        }
#endif
    }
    else if (gameInstance->lostLife)
    {
        DrawRectangleRounded({ (float)(GetScreenWidth() / 2 - 500 * gameScale), (float)(GetScreenHeight() / 2 - 40 * gameScale), 1000 * gameScale, 120 * gameScale }, 0.76f, 20 * gameScale, BLACK);
#ifdef EMSCRIPTEN_BUILD
        if (Game::isMobile) {
            DrawText("You lost a life! Tap to continue", GetScreenWidth() / 2 - 400 * gameScale, GetScreenHeight() / 2, 40 * gameScale, yellow);
        } else {
#endif
            DrawText("You lost a life! Press any key to continue", GetScreenWidth() / 2 - 400 * gameScale, GetScreenHeight() / 2, 40 * gameScale, yellow);
#ifdef EMSCRIPTEN_BUILD
        }
#endif
    }
    else if (gameInstance->isFirstStartup)
    {
        DrawRectangleRounded({ (float)(GetScreenWidth() / 2 - 500 * gameScale), (float)(GetScreenHeight() / 2 - 200 * gameScale), 1000 * gameScale, 450 * gameScale }, 0.76f, 20 * gameScale, BLACK);
        DrawText("Welcome to Space Invaders!", GetScreenWidth() / 2 - 400 * gameScale, GetScreenHeight() / 2 - 150 * gameScale, 40 * gameScale, yellow);

#ifdef EMSCRIPTEN_BUILD
        if (Game::isMobile)
        {
            // Mobile-specific controls text
            DrawText("Controls:", GetScreenWidth() / 2 - 400 * gameScale, GetScreenHeight() / 2 - 80 * gameScale, 30 * gameScale, yellow);
            DrawText("Yellow Circle (Left) - Shoot", GetScreenWidth() / 2 - 400 * gameScale, GetScreenHeight() / 2 - 30 * gameScale, 25 * gameScale, yellow);
            DrawText("Yellow Buttons (Right) - Move ship", GetScreenWidth() / 2 - 400 * gameScale, GetScreenHeight() / 2 + 10 * gameScale, 25 * gameScale, yellow);
            DrawText("Tap anywhere to start the game", GetScreenWidth() / 2 - 400 * gameScale, GetScreenHeight() / 2 + 90 * gameScale, 30 * gameScale, yellow);
        }
        else
        {
            // Desktop/browser controls text
#endif
            DrawText("Controls:", GetScreenWidth() / 2 - 400 * gameScale, GetScreenHeight() / 2 - 80 * gameScale, 30 * gameScale, yellow);
            DrawText("Arrow Keys or WASD - Move spaceship", GetScreenWidth() / 2 - 400 * gameScale, GetScreenHeight() / 2 - 30 * gameScale, 25 * gameScale, yellow);
            DrawText("Shift - Speed boost while moving", GetScreenWidth() / 2 - 400 * gameScale, GetScreenHeight() / 2 + 10 * gameScale, 25 * gameScale, yellow);
            DrawText("Space or W - Shoot", GetScreenWidth() / 2 - 400 * gameScale, GetScreenHeight() / 2 + 50 * gameScale, 25 * gameScale, yellow);
            DrawText("P - Pause game", GetScreenWidth() / 2 - 400 * gameScale, GetScreenHeight() / 2 + 90 * gameScale, 25 * gameScale, yellow);
            DrawText("ESC - Exit game", GetScreenWidth() / 2 - 400 * gameScale, GetScreenHeight() / 2 + 130 * gameScale, 25 * gameScale, yellow);
            DrawText("Press any key to start the game", GetScreenWidth() / 2 - 400 * gameScale, GetScreenHeight() / 2 + 190 * gameScale, 30 * gameScale, yellow);
#ifdef EMSCRIPTEN_BUILD
        }
#endif
        
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE) || 
            IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN) ||
            IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_S)) {
            gameInstance->isFirstStartup = false;
            gameInstance->startupDelayTimer = 0.1f;  // Set 100ms delay
            return;  // Return early to prevent the input from being processed
        }
#ifdef EMSCRIPTEN_BUILD
        if (Game::isMobile && IsGestureDetected(GESTURE_TAP)) {
            gameInstance->isFirstStartup = false;
            gameInstance->startupDelayTimer = 0.1f;  // Set 100ms delay
            return;  // Return early to prevent the input from being processed
        }
#endif
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

#ifdef EMSCRIPTEN_BUILD
    windowWidth = WEB_CANVAS_WIDTH;
    windowHeight = WEB_CANVAS_HEIGHT;
    fullscreen = false;
    SetWindowSize(windowWidth, windowHeight);
    // Adjust the gameScale if on a mobile device
    if (Game::isMobile) {
        // Use a smaller scale for mobile devices to ensure everything fits on screen
        gameScale = 0.6f;
    } else {
        gameScale = 0.7f;
    }
#else
    SetWindowSize(windowWidth - borderW, windowHeight - borderH);
    SetWindowPosition(50, 50);
    ToggleBorderlessWindowed();
    // Calculate scale for desktop build
    gameScale = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);
#endif

    gameScreenWidth = gameScreenWidth + offset;
    gameScreenHeight = gameScreenHeight + 2 * offset;
    gameTarget = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(gameTarget.texture, TEXTURE_FILTER_BILINEAR); // Texture scale filter to use
    SetTargetFPS(144);

    gameInstance = new Game();
    spaceshipImage = LoadTexture("Graphics/spaceship.png");

#ifdef EMSCRIPTEN_BUILD
    // Use emscripten_set_main_loop for Emscripten builds
    if (Game::isMobile) {
        gameScale = 1.0f;
    }
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