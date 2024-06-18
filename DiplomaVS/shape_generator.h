#pragma once
#include "globals.h"

typedef struct colorRGBA {
    float r, g, b, a;
} colorRGBA;

byte rgbGetByte(float c);

typedef struct ShapeRect {
    float x, y, w, h;
    colorRGBA fill;
    colorRGBA border;
} ShapeRect;

typedef struct ShapeTriangle {
    float x1, y1, x2, y2, x3, y3;
    colorRGBA fill;
    colorRGBA border;
} ShapeTriangle;

typedef struct ShapeCircle {
    float x, y, rad;
    colorRGBA fill;
    colorRGBA border;
} ShapeCircle;

ShapeRect generateRectangle();
ShapeTriangle generateTriangle();
ShapeCircle generateCircle();