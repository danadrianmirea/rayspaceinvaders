#pragma once

// Define EMSCRIPTEN_BUILD to enable Emscripten-specific code
// #define EMSCRIPTEN_BUILD

#ifdef EMSCRIPTEN_BUILD
#include <emscripten.h>
#endif

#define MUSIC 0

extern int gameScreenWidth;
extern int gameScreenHeight;
extern int frameOffsetLeft;
extern int frameOffsetRight;
extern float gameScale;