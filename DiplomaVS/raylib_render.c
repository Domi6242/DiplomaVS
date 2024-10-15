#ifdef DOMI_RAYLIB
#include "raylib_render.h"

#include <raylib.h>

#include "delta_time.h"
#include "globals.h"
#include "shape_generator.h"
#include "test.h"
#include "perf_counter.h"
#include <rpcndr.h>

// silly macro to convert rgba struct to raylib color struct
#define RGBA_TO_RAYLIB(c) ((Color){(byte)(c.r*255.0f + .5f), (byte)(c.g*255.0f + .5f), (byte)(c.b*255.0f + .5f), (byte)(c.a*255.0f + .5f)})

void init_raylib(RaylibObj *rt) {
    rt->shapeSelect = TEST_SHAPES_1;

    rt->imageAngleRad = IMAGE_ANGLE_RAD;
    rt->imageAngleSpeed = IMAGE_ROTATION_SPEED_RAD;
    rt->imageScale = IMAGE_SCALE;
    rt->imageScaleSpeed = IMAGE_SCALE_SPEED;

    rt->textSize = TEXT_SIZE;
    rt->textScaleSpeed = TEXT_SCALE_SPEED;

    rt->running_test = 0;
    rt->is_perf = 0;

    // Load image into texture in video memory
    Image img = LoadImage(testImagePath);

    // size the image to fit into the screen
    double ratio_x = (double)WINDOW_WIDTH / (double)img.width;
    double ratio_y = (double)WINDOW_HEIGHT / (double)img.height;
    double scale_factor = ratio_x < ratio_y ? ratio_x : ratio_y;
    ImageResize(&img, (int)(img.width * scale_factor), (int)(img.height * scale_factor));

    // load the image into memory as a texture
    rt->testImage = LoadTextureFromImage(img);
    UnloadImage(img);

    // Load font
    rt->customFont = LoadFont(testFontPath);
    GenTextureMipmaps(&rt->customFont.texture);
    SetTextureFilter(rt->customFont.texture, TEXTURE_FILTER_TRILINEAR);

    rt->test = test_init();

    // Start frame delta timer
    deltaInit();
    deltaUpdate();
}

static void draw_rectangle(ShapeRect rect) {
    DrawRectangle((int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h, RGBA_TO_RAYLIB(rect.fill));
    DrawRectangleLines((int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h, RGBA_TO_RAYLIB(rect.border));
}

static void draw_circle(ShapeCircle circle) {
    DrawCircle((int)circle.x, (int)circle.y, circle.rad, RGBA_TO_RAYLIB(circle.fill));
    DrawCircleLines((int)circle.x, (int)circle.y, circle.rad, RGBA_TO_RAYLIB(circle.border));
}

static void draw_triangle(ShapeTriangle triangle) {
    DrawTriangle(
        (Vector2) { triangle.x1, triangle.y1 },
        (Vector2) { triangle.x2, triangle.y2 },
        (Vector2) { triangle.x3, triangle.y3 },
        RGBA_TO_RAYLIB(triangle.fill));

    DrawTriangleLines(
        (Vector2) { triangle.x1, triangle.y1 },
        (Vector2) { triangle.x2, triangle.y2 },
        (Vector2) { triangle.x3, triangle.y3 },
        RGBA_TO_RAYLIB(triangle.border)
    );
}

static void test_shapes(int shapes_num) {
    for (int i = 0; i < shapes_num; i++) {
        switch (i % 3) {
        case 0:
            draw_rectangle(generateRectangle());
            break;

        case 1:
            draw_circle(generateCircle());
            break;

        case 2:
            draw_triangle(generateTriangle());
            break;

        default:
            break;
        }
    }
}

static void test_image(RaylibObj *rt, float delta) {
    if ((rt->imageScale > IMAGE_SCALE_MAX && rt->imageScaleSpeed > 0) ||
        (rt->imageScale < IMAGE_SCALE_MIN && rt->imageScaleSpeed < 0)) {
        rt->imageScaleSpeed = -rt->imageScaleSpeed;
    }

    rt->imageScale += rt->imageScaleSpeed * delta;
    rt->imageAngleRad += rt->imageAngleSpeed * delta;

    Vector2 winCenter = { WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f };
    Rectangle imgRect = {
      0.0f, 0.0f, (float)rt->testImage.width, (float)rt->testImage.height
    };
    Vector2 imgCenter = { (rt->testImage.width * rt->imageScale) / 2.0f,
                          (rt->testImage.height * rt->imageScale) / 2.0f };
    Rectangle imgDest = { winCenter.x,
                          winCenter.y,
                          imgRect.width * rt->imageScale,
                          imgRect.height * rt->imageScale };

    DrawTexturePro(rt->testImage,    // texture
        imgRect,                     // source rectangle (the entire image)
        imgDest,                     // destination rectangle on screen
        imgCenter,                   // origin offset for the source
        rt->imageAngleRad * RAD2DEG, // rotation
        WHITE);
}

static void test_text(RaylibObj *rt, float delta) {
    if ((rt->textSize > TEXT_SIZE_MAX && rt->textScaleSpeed > 0) ||
        (rt->textSize < TEXT_SIZE_MIN && rt->textScaleSpeed < 0)) {
        rt->textScaleSpeed = -rt->textScaleSpeed;
    }

    rt->textSize += rt->textScaleSpeed * delta;

    for (int i = 0; i < TEXT_REPEAT_LINES; i++) {
        DrawTextEx(
            rt->customFont,
            testText,
            (Vector2) { 5.0f, 10.0f + rt->textSize * i },
            rt->textSize, 0.0f, BLACK);
    }
}

static void run_tests(RaylibObj *rt, float delta) {
    // Initialisation
    if (rt->test.current_task == TEST_NONE) {
        rt->test = test_init();
    }

    // Update
    int test_done = test_update(&rt->test, delta);

    // Finalise
    if (test_done) {
        rt->running_test = 0;
        return;
    }

    // Draw
    switch (rt->test.current_task) {
    case TEST_SHAPES_1:
    case TEST_SHAPES_2:
    case TEST_SHAPES_3:
    case TEST_SHAPES_4:
    case TEST_SHAPES_5:
        test_shapes(shapesXPerFramePerTest[rt->test.current_task]);
        break;
    case TEST_IMAGE:
        test_image(rt, delta);
        break;
    case TEST_TEXT:
        test_text(rt, delta);
        break;
    default:
        break;
    }
}

void renderFrame(RaylibObj *rt) {
    // Update and get the delta
    float frameDelta = deltaUpdate();

    // User input
    if (IsKeyReleased(KEY_ONE)) {
        rt->running_test = 1;
    } else if (IsKeyReleased(KEY_TWO)) {
        rt->running_test = 2;
    } else if (IsKeyReleased(KEY_THREE)) {
        rt->running_test = 3;
    } else if (IsKeyReleased(KEY_ZERO)) {
        rt->running_test = 0;
    } else if (IsKeyReleased(KEY_SPACE)) {
        //rt->running_test = -1;
        if (rt->is_perf == 0) {
            rt->is_perf = 1;
            perf_counter_init(&rt->perf);
        } else {
            rt->is_perf = 0;
            perf_counter_end(&rt->perf);

            size_t len = 256;
            char buff[256];
            perf_counter_output(&rt->perf, buff, len);
            TraceLog(LOG_INFO, buff);
        }
    } else if (IsKeyReleased(KEY_EQUAL)) {
        rt->shapeSelect++;
        if (rt->shapeSelect > TEST_SHAPES_5) {
            rt->shapeSelect = TEST_SHAPES_5;
        }
    } else if (IsKeyReleased(KEY_MINUS)) {
        rt->shapeSelect--;
        if (rt->shapeSelect < TEST_SHAPES_1) {
            rt->shapeSelect = TEST_SHAPES_1;
        }
    }

    // Draw
    BeginDrawing();
    ClearBackground(WHITE);

    switch (rt->running_test) {
    case 0:
        DrawTextEx(rt->customFont, "Use keys 1, 2, 3 to cycle through the tests", (Vector2) { 10, 10 }, 32, 0.0f, BLACK);
        break;
    case 1:
        test_shapes(shapesXPerFramePerTest[rt->shapeSelect]);
        break;
    case 2:
        test_image(rt, frameDelta);
        break;
    case 3:
        test_text(rt, frameDelta);
        break;
    case -1:
        run_tests(rt, frameDelta);
        break;
    default:
        break;
    }

    EndDrawing();

    if (rt->is_perf == 1) {
        perf_counter_frame_update(&rt->perf);
    }
}
#endif // DOMI_RAYLIB