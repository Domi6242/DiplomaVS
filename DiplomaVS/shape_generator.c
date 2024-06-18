#include "shape_generator.h"

#include <stdlib.h>
#include "globals.h"

float _getRandomFloat(float a, float b) {
    return ((b - a) * ((float)rand() / RAND_MAX)) + a;
}

byte rgbGetByte(float c) {
    return (byte)(c * 255.0f + 0.5f);
}

ShapeRect generateRectangle() {
    ShapeRect rect;

    rect.w = _getRandomFloat(SHAPES_MIN_SIZE, SHAPES_MAX_SIZE);
    rect.h = _getRandomFloat(SHAPES_MIN_SIZE, SHAPES_MAX_SIZE);
    rect.x = _getRandomFloat(0.0f, WINDOW_WIDTH - rect.w);
    rect.y = _getRandomFloat(0.0f, WINDOW_HEIGHT - rect.h);

    rect.fill.r = _getRandomFloat(0.0f, 1.0f);
    rect.fill.g = _getRandomFloat(0.0f, 1.0f);
    rect.fill.b = _getRandomFloat(0.0f, 1.0f);

    rect.border.r = _getRandomFloat(0.0f, 1.0f);
    rect.border.g = _getRandomFloat(0.0f, 1.0f);
    rect.border.b = _getRandomFloat(0.0f, 1.0f);

#ifdef SHAPES_USE_ALPHA
    rect.fill.a = _getRandomFloat(SHAPES_MIN_ALPHA, SHAPES_MAX_ALPHA);
    rect.border.a = _getRandomFloat(SHAPES_MIN_ALPHA, SHAPES_MAX_ALPHA);

#else
    rect.fill.a = 1.0;
    rect.border.a = 1.0;
#endif

    return rect;
}

#define TRI_RATIO 0.86602540378f

ShapeTriangle generateTriangle() {
    float size = _getRandomFloat(SHAPES_MIN_SIZE, SHAPES_MAX_SIZE);
    float height = TRI_RATIO * size;

    ShapeTriangle tri;

    /**
     *
     *     3
     *    / \
     *   /   \
     *  /     \
     * 1-------2
     *
    */

    tri.x1 = _getRandomFloat(0.0f, WINDOW_WIDTH - size);
    tri.y1 = _getRandomFloat(0.0f + height, WINDOW_HEIGHT);
    tri.x2 = tri.x1 + size;
    tri.y2 = tri.y1;
    tri.x3 = tri.x1 + size * 0.5f;
    tri.y3 = tri.y1 - height;

    tri.fill.r = _getRandomFloat(0.0f, 1.0f);
    tri.fill.g = _getRandomFloat(0.0f, 1.0f);
    tri.fill.b = _getRandomFloat(0.0f, 1.0f);

    tri.border.r = _getRandomFloat(0.0f, 1.0f);
    tri.border.g = _getRandomFloat(0.0f, 1.0f);
    tri.border.b = _getRandomFloat(0.0f, 1.0f);

#ifdef SHAPES_USE_ALPHA
    tri.fill.a = _getRandomFloat(SHAPES_MIN_ALPHA, SHAPES_MAX_ALPHA);
    tri.border.a = _getRandomFloat(SHAPES_MIN_ALPHA, SHAPES_MAX_ALPHA);
#else
    tri.fill.a = 1.0;
    tri.border.a = 1.0;
#endif

    return tri;
}

ShapeCircle generateCircle() {

    ShapeCircle circle;
    circle.rad = _getRandomFloat(SHAPES_MIN_SIZE * 0.5f, SHAPES_MAX_SIZE * 0.5f);
    circle.x = _getRandomFloat(circle.rad, WINDOW_WIDTH - circle.rad);
    circle.y = _getRandomFloat(circle.rad, WINDOW_HEIGHT - circle.rad);

    circle.fill.r = _getRandomFloat(0.0f, 1.0f);
    circle.fill.g = _getRandomFloat(0.0f, 1.0f);
    circle.fill.b = _getRandomFloat(0.0f, 1.0f);

    circle.border.r = _getRandomFloat(0.0f, 1.0f);
    circle.border.g = _getRandomFloat(0.0f, 1.0f);
    circle.border.b = _getRandomFloat(0.0f, 1.0f);

#ifdef SHAPES_USE_ALPHA
    circle.fill.a = _getRandomFloat(SHAPES_MIN_ALPHA, SHAPES_MAX_ALPHA);
    circle.border.a = _getRandomFloat(SHAPES_MIN_ALPHA, SHAPES_MAX_ALPHA);
#else
    circle.fill.a = 1.0;
    circle.border.a = 1.0;
#endif

    return circle;
}