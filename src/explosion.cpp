#include "explosion.h"

Texture2D Explosion::explosionTexture;

Explosion::Explosion(Vector2 pos, float scale) : position(pos), active(true), frameTimer(0.0f), currentFrame(0), scale(scale) {
    if (explosionTexture.id == 0) {
        explosionTexture = LoadTexture("Graphics/explosion.png");
    }
}

void Explosion::Update() {
    if (!active) return;

    frameTimer += GetFrameTime();
    if (frameTimer >= FRAME_DURATION) {
        frameTimer = 0.0f;
        currentFrame++;
        if (currentFrame >= FRAME_COUNT) {
            active = false;
        }
    }
}

void Explosion::Draw() {
    if (!active) return;

    int row = currentFrame / 5;
    int col = currentFrame % 5;
    Rectangle sourceRec = {
        (float)(col * FRAME_SIZE),
        (float)(row * FRAME_SIZE),
        (float)FRAME_SIZE,
        (float)FRAME_SIZE
    };
    float scaledSize = FRAME_SIZE * scale;  // Use the scale parameter
    Rectangle destRec = {
        position.x - scaledSize/2,  // Center the explosion
        position.y - scaledSize/2,
        scaledSize,
        scaledSize
    };
    DrawTexturePro(explosionTexture, sourceRec, destRec, {0, 0}, 0.0f, WHITE);
} 