#include <string>
#include <raylib.h>
#include "globals.h"
#include "game.h"

int main()
{
    InitWindow(gameScreenWidth, gameScreenHeight, "Space invaders");
    InitAudioDevice();
    SetMasterVolume(0.22f);
    SetExitKey(KEY_NULL);

    gameScreenWidth = gameScreenWidth + offset;
    gameScreenHeight = gameScreenHeight + 2 * offset;
     
    SetTargetFPS(144);
    Game game;

    ToggleBorderlessWindowed();
    float dt = GetFrameTime();

    while (!exitWindow)
    {
        // todo scale game correctly using dt, seems to run differently on different machines
        dt = GetFrameTime();
        game.Update(dt);
        game.Draw();
    }

    CloseWindow();
    
    return 0;
}