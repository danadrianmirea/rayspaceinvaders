#include "explosion.h"

Texture2D Explosion::explosionTexture;

Explosion::Explosion(Vector2 pos) : position(pos), active(true), frameTimer(0.0f), currentFrame(0) {
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
    float halfSize = FRAME_SIZE / 2.0f;  // Half the original size
    Rectangle destRec = {
        position.x - halfSize/2,  // Center the smaller explosion
        position.y - halfSize/2,
        halfSize,  // Use half size
        halfSize   // Use half size
    };
    DrawTexturePro(explosionTexture, sourceRec, destRec, {0, 0}, 0.0f, WHITE);
} 