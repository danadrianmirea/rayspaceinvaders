#pragma once

#include "raylib.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

extern int gameScreenWidth;
extern int gameScreenHeight;
extern int frameOffsetLeft;
extern int frameOffsetRight;

extern bool exitWindowRequested; 
extern bool exitWindow;
extern bool fullscreen;
extern const int borderH;
extern const int borderW;


extern int windowWidth;
extern int windowHeight;

extern Color grey;
extern Color yellow;

extern int offset;
