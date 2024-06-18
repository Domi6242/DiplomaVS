#pragma once

#ifdef DOMI_RAYLIB
#include <raylib.h>

typedef struct RaylibObj {
    Texture2D testImage;
    float imageAngleRad;
    float imageAngleSpeed;
    float imageScale;
    float imageScaleSpeed;

    Font customFont;
    float textSize;
    float textScaleSpeed;

    int running_test;
} RaylibObj;

void renderFrame(RaylibObj* rt);
void init_raylib(RaylibObj* rt);
#endif  // DOMI_RAYLIB