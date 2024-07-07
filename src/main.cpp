#include <string>
#include <raylib.h>
#include "globals.h"
#include "game.h"

int main()
{
    InitWindow(gameScreenWidth, gameScreenHeight, "Space invaders");
    SetExitKey(KEY_NULL); // Disable KEY_ESCAPE to close window, X-button still works
    //  int display = GetCurrentMonitor();
    //  int windowWidth = (int)(GetMonitorWidth(display));
    //  int windowHeight = (int)(GetMonitorHeight(display));
    //  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);.
    SetWindowSize(windowWidth - borderW, windowHeight - borderH);
    // MaximizeWindow();
    SetWindowPosition(50, 50);
    // ToggleFullscreen();
    
    gameScreenWidth = gameScreenWidth + offset;
    gameScreenHeight = gameScreenHeight + 2 * offset;
     
    SetTargetFPS(144);
    Game game;

    ToggleBorderlessWindowed();

    while (!exitWindow)
    {
        game.Update();
        game.Draw();
    }

    CloseWindow();
    
    return 0;
}