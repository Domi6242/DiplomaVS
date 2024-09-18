#pragma once

//#include "globals.h"
#ifdef DOMI_RAYLIB
#define NOGDI   // All GDI defines and routines
#define NOUSER  // All USER defines and routines
#endif

#include <Windows.h>  // or any library that uses Windows.h

#ifdef DOMI_RAYLIB  // raylib uses these names as function parameters
#undef near
#undef far
#endif

typedef enum TestType {
    TEST_NONE,
    TEST_SHAPES_1,
    TEST_SHAPES_2,
    TEST_SHAPES_3,
    TEST_SHAPES_4,
    TEST_SHAPES_5,
    TEST_IMAGE,
    TEST_TEXT,
    TEST_COUNT
} TestType;

static const WCHAR *testName[TEST_COUNT] = {
    L"None",
    L"Shapes 1",
    L"Shapes 2",
    L"Shapes 3",
    L"Shapes 4",
    L"Shapes 5",
    L"Image",
    L"Text"
};

typedef struct Test {
    TestType current_task;
    unsigned long long task_start_ms;

    // frame time helpers
    unsigned long long task_frame_count;
    float task_total_time;

    // memory helpers
    float task_mem_sum;

    // data storage
    float avg_times_ms[TEST_COUNT];
    float slowest_frame_ms[TEST_COUNT];
    float avg_mem_usage[TEST_COUNT];
} Test;

Test test_init();
int test_update(Test* test, float delta);