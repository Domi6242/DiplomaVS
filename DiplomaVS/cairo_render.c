#ifdef DOMI_CAIRO
#include "cairo_render.h"

#include <cairo.h>
#include <cairo-win32.h>
#include <math.h>
#include <windows.h>

#include "delta_time.h"
#include "globals.h"
#include "shape_generator.h"

void init_cairo(CairoObj *rt) {
    // Skip if already initialised
    if (rt->winSurface) {
        return;
    }

    rt->imageAngleRad = IMAGE_ANGLE_RAD;
    rt->imageAngleSpeed = IMAGE_ROTATION_SPEED_RAD;
    rt->imageScale = IMAGE_SCALE;
    rt->imageScaleSpeed = IMAGE_SCALE_SPEED;

    rt->textSize = TEXT_SIZE;
    rt->textScaleSpeed = TEXT_SCALE_SPEED;

    rt->running_test = 0;

    // Load image from disk
    rt->test_image.source = cairo_image_surface_create_from_png(testImagePath);
    int src_width = cairo_image_surface_get_width(rt->test_image.source);
    int src_height = cairo_image_surface_get_height(rt->test_image.source);

    // size the image to fit into the screen
    double ratio_x = (double)WINDOW_WIDTH / (double)src_width;
    double ratio_y = (double)WINDOW_HEIGHT / (double)src_height;
    double scale_factor = ratio_x < ratio_y ? ratio_x : ratio_y;
    rt->test_image.width = src_width / scale_factor;
    rt->test_image.height = src_height / scale_factor;

    // Resize the source image and set it's origin to be centered
    cairo_surface_set_device_offset(rt->test_image.source, src_width / 2.0, src_height / 2.0);
    cairo_surface_set_device_scale(rt->test_image.source, 1.0 / scale_factor, 1.0 / scale_factor);

    // Start frame delta timer
    deltaInit();
    deltaUpdate();
}

static void draw_rectangle(CairoObj *rt, ShapeRect rect) {
    cairo_rectangle(rt->bufferContext, (int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h);

    cairo_set_source_rgba(rt->bufferContext, rect.fill.r, rect.fill.g, rect.fill.b, rect.fill.a);
    cairo_fill_preserve(rt->bufferContext);

    cairo_set_source_rgba(rt->bufferContext, rect.border.r, rect.border.g, rect.border.b, rect.border.a);
    cairo_set_line_width(rt->bufferContext, 1.0f);
    cairo_stroke(rt->bufferContext);
}

static void draw_circle(CairoObj *rt, ShapeCircle circle) {
    cairo_arc(rt->bufferContext, (int)circle.x, (int)circle.y, (int)circle.rad, 0, 2 * M_PI);

    cairo_set_source_rgba(rt->bufferContext, circle.fill.r, circle.fill.g, circle.fill.b, circle.fill.a);
    cairo_fill_preserve(rt->bufferContext);

    cairo_set_source_rgba(rt->bufferContext, circle.border.r, circle.border.g, circle.border.b, circle.border.a);
    cairo_set_line_width(rt->bufferContext, 1.0f);
    cairo_stroke(rt->bufferContext);
}

static void draw_triangle(CairoObj *rt, ShapeTriangle triangle) {
    cairo_move_to(rt->bufferContext, (int)triangle.x1, (int)triangle.y1);
    cairo_line_to(rt->bufferContext, (int)triangle.x2, (int)triangle.y2);
    cairo_line_to(rt->bufferContext, (int)triangle.x3, (int)triangle.y3);
    cairo_close_path(rt->bufferContext);

    cairo_set_source_rgba(rt->bufferContext, triangle.fill.r, triangle.fill.g, triangle.fill.b, triangle.fill.a);
    cairo_fill_preserve(rt->bufferContext);

    cairo_set_source_rgba(rt->bufferContext, triangle.border.r, triangle.border.g, triangle.border.b, triangle.border.a);
    cairo_set_line_width(rt->bufferContext, 1.0f);
    cairo_stroke(rt->bufferContext);
}

static void test_shapes(CairoObj *rt) {
    for (int i = 0; i < 1000; i++) {
        switch (i % 3) {
        case 0:
            draw_rectangle(rt, generateRectangle());
            break;
        case 1:
            draw_circle(rt, generateCircle());
            break;
        case 2:
            draw_triangle(rt, generateTriangle());
            break;
        default:
            break;
        }
    }
}

static void test_image(CairoObj *rt, float delta) {
    if ((rt->imageScale > IMAGE_SCALE_MAX && rt->imageScaleSpeed > 0) ||
        (rt->imageScale < IMAGE_SCALE_MIN && rt->imageScaleSpeed < 0)) {
        rt->imageScaleSpeed = -rt->imageScaleSpeed;
    }

    rt->imageScale += rt->imageScaleSpeed * delta;
    rt->imageAngleRad += rt->imageAngleSpeed * delta;

    // Calculate the transformaiton matrix:
    // Center, rotate and scale
    cairo_matrix_t matrix;
    cairo_matrix_init_identity(&matrix);
    cairo_matrix_translate(&matrix, WINDOW_WIDTH / 2.0, WINDOW_HEIGHT / 2.0);
    cairo_matrix_rotate(&matrix, rt->imageAngleRad);
    cairo_matrix_scale(&matrix, rt->imageScale, rt->imageScale);
    cairo_matrix_invert(&matrix);

    // Apply transformation matrix to a copy of the image
    cairo_pattern_t *img = cairo_pattern_create_for_surface(rt->test_image.source);
    cairo_pattern_set_filter(img, CAIRO_FILTER_FAST);
    cairo_pattern_set_matrix(img, &matrix);

    // Apply the transformed image to the buffer
    cairo_set_source(rt->bufferContext, img);
    cairo_paint(rt->bufferContext);
    cairo_pattern_destroy(img);
}

static void draw_text(CairoObj *rt, double x, double y, const char *text, double font_size) {
    cairo_set_source_rgb(rt->bufferContext, 0.0, 0.0, 0.0);
    cairo_select_font_face(rt->bufferContext, "Iosevka Etoile", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(rt->bufferContext, font_size);

    cairo_move_to(rt->bufferContext, x, y);
    cairo_show_text(rt->bufferContext, text);
}

static void test_text(CairoObj *rt, float delta) {
    if ((rt->textSize > TEXT_SIZE_MAX && rt->textScaleSpeed > 0) ||
        (rt->textSize < TEXT_SIZE_MIN && rt->textScaleSpeed < 0)) {
        rt->textScaleSpeed = -rt->textScaleSpeed;
    }

    rt->textSize += rt->textScaleSpeed * delta;

    for (int i = 0; i < TEXT_REPEAT_LINES; i++) {
        draw_text(rt, 5, rt->textSize * (i + 1), testText, rt->textSize);
    }
}

void deinit_cairo(CairoObj *rt) { 
    //cairo_destroy(rt->test_image.source); 
}

void render_frame(CairoObj *rt) {
    init_cairo(rt);

    // Update and get the delta
    float frameDelta = deltaUpdate();
    if (!isfinite(frameDelta)) {
        frameDelta = 0;
    }

    // Create the cairo surface on which we draw
    HDC hdc = GetDC(rt->hwnd);
    rt->winSurface    = cairo_win32_surface_create_with_format(hdc, CAIRO_FORMAT_ARGB32);
    //rt->winSurface = cairo_win32_surface_create_with_ddb(hdc, CAIRO_FORMAT_RGB24, WINDOW_WIDTH, WINDOW_HEIGHT);
    rt->bufferSurface = cairo_surface_create_similar_image(rt->winSurface, CAIRO_FORMAT_RGB24, WINDOW_WIDTH, WINDOW_HEIGHT);
    rt->winContext    = cairo_create(rt->winSurface);
    rt->bufferContext = cairo_create(rt->bufferSurface);

    // Prepare the buffer context for drawing and clear the screen
    cairo_set_antialias(rt->bufferContext, CAIRO_ANTIALIAS_NONE);
    cairo_set_source_rgb(rt->bufferContext, 1.0, 1.0, 1.0);
    cairo_paint(rt->bufferContext);

    // Render scene to buffer context
    switch (rt->running_test) {
    case 0:
        draw_text(rt, 5, 42, "Use keys 1, 2, 3 to cycle through the tests", 32);
        break;
    case 1:
        test_shapes(rt);
        break;
    case 2:
        test_image(rt, frameDelta);
        break;
    case 3:
        test_text(rt, frameDelta);
        break;
    default:
        break;
    }

    // Finalise drawing
    cairo_set_operator(rt->winContext, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_surface(rt->winContext, rt->bufferSurface, 0, 0);
    cairo_paint(rt->winContext);

    // free up resources
    cairo_destroy(rt->bufferContext);
    cairo_destroy(rt->winContext);
    cairo_surface_destroy(rt->bufferSurface);
    cairo_surface_destroy(rt->winSurface);

    ReleaseDC(rt->hwnd, hdc);
}

#endif  // DOMI_CAIRO
