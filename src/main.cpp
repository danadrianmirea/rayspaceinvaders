#include <string>
#include <raylib.h>
// #include "raymath.h"
#include "game.h"
#include "globals.h"
#include "laser.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

bool exitWindowRequested = false; // Flag to request window to exit
bool exitWindow = false;          // Flag to set window to exit
bool fullscreen = false;
const int borderH = 100;
const int borderW = (int)(1920.0f / 1080.f * borderH);

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
int offset = 50;

// Global variables for the game loop
Game* gameInstance = nullptr;
Font gameFont;
RenderTexture2D gameTarget;
Texture2D spaceshipImage;
Texture2D backgroundTexture;  // Add background texture

void UpdateWindow(Game& game)
{
    if (game.isFirstStartup)
    {
        if (GetKeyPressed() != KEY_NULL) {
            gameInstance->isFirstStartup = false;
            gameInstance->startupDelayTimer = 0.1f;
            return; 
        }

        if (Game::isMobile && IsGestureDetected(GESTURE_TAP)) {
            gameInstance->isFirstStartup = false;
            gameInstance->startupDelayTimer = 0.1f;
            return;
        }
    }

#ifndef EMSCRIPTEN_BUILD
    if (WindowShouldClose() || (IsKeyPressed(KEY_ESCAPE) && exitWindowRequested == false && !game.gameOver && !game.lostLife && !game.lostWindowFocus))
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
            ToggleBorderlessWindowed();
        }
        else
        {
            fullscreen = true;
            ToggleBorderlessWindowed();
        }
    }
#endif

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
    if (exitWindowRequested == false && game.lostWindowFocus == false && game.lostLife == false && game.gameOver == false && IsKeyPressed(KEY_P))
#else
    if (exitWindowRequested == false && game.lostWindowFocus == false && game.lostLife == false && game.gameOver == false && !Game::isMobile && (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)))
#endif
    {
        game.paused = !game.paused;
    }
}

// Main game loop function that will be called by emscripten_set_main_loop
void GameLoop()
{
    UpdateMusicStream(gameInstance->music);

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

    // Draw background texture
    DrawTexture(backgroundTexture, 0, 0, WHITE);
    
    const int uiFontSize = 28;
    const int toggleMusicFontSize = 28;

    DrawLineEx({ 25, 730 - 50 }, { 775, 730 - 50 }, 3 * gameScale, WHITE);

    DrawTextEx(gameFont, "SCORE", { 50, 15 }, uiFontSize * gameScale, 2 * gameScale, WHITE);
    std::string scoreText = FormatWithLeadingZeroes(gameInstance->score, 7);
    DrawTextEx(gameFont, scoreText.c_str(), { 50, 40 }, uiFontSize * gameScale, 2 * gameScale, WHITE);

    // Draw music toggle status
    const char* musicStatus = gameInstance->isMusicMuted ? "M: music(OFF)" : "M: music(ON)";
    int textWidth = MeasureTextEx(gameFont, musicStatus, toggleMusicFontSize * gameScale, 2 * gameScale).x;
    DrawTextEx(gameFont, musicStatus, { (float)(gameScreenWidth - textWidth) / 2, 15 }, toggleMusicFontSize * gameScale, 2 * gameScale, WHITE);

    DrawTextEx(gameFont, "HIGH-SCORE", { 570, 15 }, uiFontSize * gameScale, 2 * gameScale, WHITE);
    std::string highScoreText = FormatWithLeadingZeroes(gameInstance->highScore, 7);
    DrawTextEx(gameFont, highScoreText.c_str(), { 570, 40 }, uiFontSize * gameScale, 2 * gameScale, WHITE);
#ifdef EMSCRIPTEN_BUILD
    float x = (float)gameScreenWidth/2 - 180;
#else
    float x = (float)gameScreenWidth/2 - 120;
#endif

    std::string levelText = "LEVEL " + FormatWithLeadingZeroes(gameInstance->currentLevel, 2);
    DrawTextEx(gameFont, levelText.c_str(), { x, 740 }, uiFontSize * gameScale, 2 * gameScale, WHITE);

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
        DrawRectangleRounded({ (float)(GetScreenWidth() / 2 - 500 * gameScale * uiScale), (float)(GetScreenHeight() / 2 - 40 * gameScale * uiScale), 1000 * gameScale * uiScale, 120 * gameScale * uiScale }, 0.76f, 20 * gameScale * uiScale, BLACK);
        DrawText("Are you sure you want to exit? [Y/N]", GetScreenWidth() / 2 - 400 * gameScale * uiScale, GetScreenHeight() / 2, 40 * gameScale * uiScale, WHITE);
    }
    else if (gameInstance->paused)
    {
        DrawRectangleRounded({ (float)(GetScreenWidth() / 2 - 550 * gameScale * uiScale), (float)(GetScreenHeight() / 2 - 40 * gameScale * uiScale), 1100 * gameScale * uiScale, 120 * gameScale * uiScale }, 0.76f, 20 * gameScale * uiScale, BLACK);
#ifdef EMSCRIPTEN_BUILD
        if (Game::isMobile) {
            DrawText("Game paused, tap center of screen to continue", GetScreenWidth() / 2 - 450 * gameScale * uiScale, GetScreenHeight() / 2, 40 * gameScale * uiScale, WHITE);
        } else {
            DrawText("Game paused, press P or ESC to continue", GetScreenWidth() / 2 - 350 * gameScale * uiScale, GetScreenHeight() / 2, 40 * gameScale * uiScale, WHITE);
        }
#else
        DrawText("Game paused, press P to continue", GetScreenWidth() / 2 - 350 * gameScale * uiScale, GetScreenHeight() / 2, 40 * gameScale * uiScale, WHITE);
#endif
    }
    else if (gameInstance->lostWindowFocus)
    {
        DrawRectangleRounded({ (float)(GetScreenWidth() / 2 - 500 * gameScale * uiScale), (float)(GetScreenHeight() / 2 - 40 * gameScale * uiScale), 1000 * gameScale * uiScale, 120 * gameScale * uiScale }, 0.76f, 20 * gameScale * uiScale, BLACK);
        DrawText("Game paused, focus window to continue", GetScreenWidth() / 2 - 400 * gameScale * uiScale, GetScreenHeight() / 2, 40 * gameScale * uiScale, WHITE);
    }
    else if (gameInstance->gameOver)
    {
        DrawRectangleRounded({ (float)(GetScreenWidth() / 2 - 500 * gameScale * uiScale), (float)(GetScreenHeight() / 2 - 40 * gameScale * uiScale), 1000 * gameScale * uiScale, 120 * gameScale * uiScale }, 0.76f, 20 * gameScale * uiScale, BLACK);
#ifdef EMSCRIPTEN_BUILD
        if (Game::isMobile) {
            DrawText("Game over, tap to play again", GetScreenWidth() / 2 - 400 * gameScale * uiScale, GetScreenHeight() / 2, 40 * gameScale * uiScale, WHITE);
        } else {
#endif
            DrawText("Game over, press any key to play again", GetScreenWidth() / 2 - 400 * gameScale * uiScale, GetScreenHeight() / 2, 40 * gameScale * uiScale, WHITE);
#ifdef EMSCRIPTEN_BUILD
        }
#endif
    }
    else if (gameInstance->lostLife)
    {
        DrawRectangleRounded({ (float)(GetScreenWidth() / 2 - 500 * gameScale * uiScale), (float)(GetScreenHeight() / 2 - 40 * gameScale * uiScale), 1000 * gameScale * uiScale, 120 * gameScale * uiScale }, 0.76f, 20 * gameScale * uiScale, BLACK);
#ifdef EMSCRIPTEN_BUILD
        if (Game::isMobile) {
            DrawText("You lost a life! Tap to continue", GetScreenWidth() / 2 - 400 * gameScale * uiScale, GetScreenHeight() / 2, 40 * gameScale * uiScale, WHITE);
        } else {
#endif
            DrawText("You lost a life! Press any key to continue", GetScreenWidth() / 2 - 400 * gameScale * uiScale, GetScreenHeight() / 2, 40 * gameScale * uiScale, WHITE);
#ifdef EMSCRIPTEN_BUILD
        }
#endif
    }
    else if (gameInstance->isFirstStartup)
    {
        DrawRectangleRounded({ (float)(GetScreenWidth() / 2 - 500 * gameScale * uiScale), (float)(GetScreenHeight() / 2 - 200 * gameScale * uiScale), 1000 * gameScale * uiScale, 550 * gameScale * uiScale }, 0.76f, 20 * gameScale * uiScale, BLACK);
        DrawText("Welcome to Space Invaders!", GetScreenWidth() / 2 - 400 * gameScale * uiScale, GetScreenHeight() / 2 - 150 * gameScale * uiScale, 40 * gameScale * uiScale, WHITE);

#ifdef EMSCRIPTEN_BUILD
        if (Game::isMobile)
        {
            // Mobile-specific controls text
            DrawText("Controls:", GetScreenWidth() / 2 - 400 * gameScale * uiScale, GetScreenHeight() / 2 - 80 * gameScale * uiScale, 30 * gameScale * uiScale, WHITE);
            DrawText("Left Circle - Shoot", GetScreenWidth() / 2 - 400 * gameScale * uiScale, GetScreenHeight() / 2 - 30 * gameScale * uiScale, 25 * gameScale * uiScale, WHITE);
            DrawText("Right Buttons - Move ship", GetScreenWidth() / 2 - 400 * gameScale * uiScale, GetScreenHeight() / 2 + 10 * gameScale * uiScale, 25 * gameScale * uiScale, WHITE);
            DrawText("Tap center of screen to pause/unpause", GetScreenWidth() / 2 - 400 * gameScale * uiScale, GetScreenHeight() / 2 + 50 * gameScale * uiScale, 30 * gameScale * uiScale, WHITE);
            DrawText("Tap anywhere to start the game", GetScreenWidth() / 2 - 400 * gameScale * uiScale, GetScreenHeight() / 2 + 130 * gameScale * uiScale, 30 * gameScale * uiScale, WHITE);
        }
        else
        {
            // Desktop/browser controls text
#endif
            DrawText("Controls:", GetScreenWidth() / 2 - 400 * gameScale * uiScale, GetScreenHeight() / 2 - 80 * gameScale * uiScale, 30 * gameScale * uiScale, WHITE);
            DrawText("Arrow Keys or WASD - Move spaceship", GetScreenWidth() / 2 - 400 * gameScale * uiScale, GetScreenHeight() / 2 - 30 * gameScale * uiScale, 25 * gameScale * uiScale, WHITE);
            DrawText("Shift - Speed boost while moving", GetScreenWidth() / 2 - 400 * gameScale * uiScale, GetScreenHeight() / 2 + 10 * gameScale * uiScale, 25 * gameScale * uiScale, WHITE);
            DrawText("Space or W - Shoot", GetScreenWidth() / 2 - 400 * gameScale * uiScale, GetScreenHeight() / 2 + 50 * gameScale * uiScale, 25 * gameScale * uiScale, WHITE);
            DrawText("M - Toggle music on/off", GetScreenWidth() / 2 - 400 * gameScale * uiScale, GetScreenHeight() / 2 + 90 * gameScale * uiScale, 25 * gameScale * uiScale, WHITE);
#ifndef EMSCRIPTEN_BUILD
            DrawText("P - Pause game", GetScreenWidth() / 2 - 400 * gameScale * uiScale, GetScreenHeight() / 2 + 130 * gameScale * uiScale, 25 * gameScale * uiScale, WHITE);
            DrawText("ESC - Exit game", GetScreenWidth() / 2 - 400 * gameScale * uiScale, GetScreenHeight() / 2 + 170 * gameScale * uiScale, 25 * gameScale * uiScale, WHITE);
#else
            DrawText("P or ESC - Pause game", GetScreenWidth() / 2 - 400 * gameScale * uiScale, GetScreenHeight() / 2 + 130 * gameScale * uiScale, 30 * gameScale * uiScale, WHITE);
#endif            
#ifndef EMSCRIPTEN_BUILD
            DrawText("ALT + ENTER - Toggle fullscreen", GetScreenWidth() / 2 - 400 * gameScale * uiScale, GetScreenHeight() / 2 + 210 * gameScale * uiScale, 30 * gameScale * uiScale, WHITE);
#endif
            DrawText("Press any key to start the game", GetScreenWidth() / 2 - 400 * gameScale * uiScale, GetScreenHeight() / 2 + 260 * gameScale * uiScale, 30 * gameScale * uiScale, WHITE);
#ifdef EMSCRIPTEN_BUILD
        }
#endif
    }

    EndDrawing();
}

int main()
{
    InitWindow(gameScreenWidth, gameScreenHeight, "Space invaders");
#ifndef EMSCRIPTEN_BUILD
    SetWindowState(FLAG_WINDOW_RESIZABLE);
#endif
    InitAudioDevice();
    SetExitKey(KEY_NULL); // Disable KEY_ESCAPE to close window, X-button still works

    gameFont = LoadFontEx("Font/monogram.ttf", 64, 0, 0);

    // Load background texture
    backgroundTexture = LoadTexture("Graphics/background.png");

#ifdef EMSCRIPTEN_BUILD
    fullscreen = false;
#else
    SetWindowSize(windowWidth - borderW, windowHeight - borderH);
    SetWindowPosition(50, 50);
    if(fullscreen) 
    {
        ToggleBorderlessWindowed();
    }
    // Calculate scale for desktop build
    gameScale = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);
#endif

    gameScreenWidth = gameScreenWidth + offset;
    gameScreenHeight = gameScreenHeight + 2 * offset;
    gameTarget = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(gameTarget.texture, TEXTURE_FILTER_BILINEAR); // Texture scale filter to use
    SetTargetFPS(144);

    gameInstance = new Game();
    Image img = LoadImage("Graphics/spaceship.png");
    ImageResize(&img, uiSpaceshipSize, (int)((float)img.height * ((float)uiSpaceshipSize / (float)img.width)));
    spaceshipImage = LoadTextureFromImage(img);
    UnloadImage(img);
    
    // Load laser texture
    img = LoadImage("Graphics/laser.png");
    ImageResize(&img, laserWidth, laserHeight);
    Laser::laserTexture = LoadTextureFromImage(img);
    UnloadImage(img);

    // Load alien laser texture
    img = LoadImage("Graphics/alien_laser.png");
    ImageResize(&img, laserWidth, laserHeight);
    Laser::alienLaserTexture = LoadTextureFromImage(img);
    UnloadImage(img);

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
    UnloadTexture(Laser::laserTexture);
    UnloadTexture(Laser::alienLaserTexture);
    UnloadTexture(backgroundTexture);  // Unload background texture
    UnloadRenderTexture(gameTarget);
    CloseAudioDevice();
    return 0;
}