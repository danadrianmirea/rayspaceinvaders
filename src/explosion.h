#pragma once

#include "raylib.h"

class Explosion {
public:
    Explosion(Vector2 pos);
    void Update();
    void Draw();
    bool IsActive() const { return active; }

private:
    Vector2 position;
    bool active;
    float frameTimer;
    int currentFrame;
    static Texture2D explosionTexture;
    static const int FRAME_COUNT = 25;  // 5x5 texture atlas
    static constexpr float FRAME_DURATION = 0.02f;  // 20ms per frame (50% faster)
    static const int FRAME_SIZE = 204;  // 1024/5 = 204 pixels per frame
}; 