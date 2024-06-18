#include "delta_time.h"

#include <windows.h>

static LARGE_INTEGER d_frequency, d_start, d_end, d_counts, d_fps, d_ms;

void deltaInit() {
    QueryPerformanceCounter(&d_start);
    QueryPerformanceFrequency(&d_frequency);
}

float deltaUpdate() {
    QueryPerformanceCounter(&d_end);
    float delta_seconds = ((float)(d_end.QuadPart - d_start.QuadPart) / (float)d_frequency.QuadPart);
    d_start = d_end;
    return delta_seconds;
}