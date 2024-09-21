#pragma once

#ifdef DOMI_CAIRO
#include <Windows.h>
#include <cairo.h>
#include "test.h"

typedef struct CairoImage {
    cairo_surface_t *source;
    int width, height;
} CairoImage;

typedef struct CairoObj {
    HWND hwnd;
    cairo_surface_t *winSurface;
    cairo_surface_t *bufferSurface;
    cairo_t *bufferContext;
    cairo_t *winContext;

    int shapeSelect;

    //cairo_surface_t *testImage;
    CairoImage test_image;
    float imageAngleRad;
    float imageAngleSpeed;
    float imageScale;
    float imageScaleSpeed;

    float textSize;
    float textScaleSpeed;

    Test test;
    int running_test;
} CairoObj;

void init_cairo(CairoObj *rt);
void deinit_cairo(CairoObj *rt);

void render_frame(CairoObj *rt);
#endif  // DOMI_CAIRO