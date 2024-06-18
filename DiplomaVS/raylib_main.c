#ifdef DOMI_RAYLIB
#include <raylib.h>
#include <stdio.h>
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
#include "perf_log.h"
#include "perf_tracker.h"
#include "raylib_render.h"

Test currentRuningTest = TEST_SHAPES;

int main(int argc, LPWSTR argv[]) {
    (void)argc;
    srand((unsigned int)time(0));

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raylib Example");

    if (!logInit(argv[0])) {
        wprintf(L"Write init failed!\n");
        system("pause");
        return 1;
    }

    RaylibObj raylibObj;

    // Main loop
    init_raylib(&raylibObj);
    perfInit();
    while (!WindowShouldClose()) {
        perfStart();
        renderFrame(&raylibObj);
        perfStop();
    }

    // De-Initialization
    // Close window and OpenGL context
    CloseWindow();

    logWriteLogSection(L"Section 1: Shapes");

    if (!logClose()) {
        wprintf(L"Close failed!\n");
        system("pause");
        return 1;
    }

    return 0;
}
#endif  // DOMI_RAYLIB


