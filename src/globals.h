#pragma once

#ifdef EMSCRIPTEN_BUILD
#include <emscripten.h>
#endif

extern int gameScreenWidth;
extern int gameScreenHeight;
extern int frameOffsetLeft;
extern int frameOffsetRight;
extern const int alienSize;
extern const int spaceshipSize;
extern const int misteryShipSize;
extern const int uiSpaceshipSize;
extern const int laserWidth;
extern const int laserHeight;
extern float gameScale;
extern float uiScale;
extern const int targetFps;
