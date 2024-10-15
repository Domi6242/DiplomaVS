#ifdef DOMI_RAYLIB
#include <raylib.h>
#include <stdlib.h>
#include <time.h>

#if defined(_WIN32)
#define NOGDI   // All GDI defines and routines
#define NOUSER  // All USER defines and routines
#endif

#include <Windows.h>  // or any library that uses Windows.h

#if defined(_WIN32)  // raylib uses these names as function parameters
#undef near
#undef far
#endif

#include "globals.h"
#include "raylib_render.h"

int main(int argc, LPWSTR argv[]) {
    (void)argc;
    srand((unsigned int)time(0));

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raylib Example");

    RaylibObj raylibObj;

    // Main loop
    init_raylib(&raylibObj);
    while (!WindowShouldClose()) {
        renderFrame(&raylibObj);
    }

    // De-Initialization
    // Close window and OpenGL context
    CloseWindow();

    return 0;
}
#endif  // DOMI_RAYLIB


