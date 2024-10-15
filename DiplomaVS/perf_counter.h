#pragma once

#ifdef DOMI_RAYLIB
#define NOGDI   // All GDI defines and routines
#define NOUSER  // All USER defines and routines
#endif

#include <Windows.h>

#ifdef DOMI_RAYLIB  // raylib uses these names as function parameters
#undef near
#undef far
#endif

typedef struct PerfCounter {
    float frame_min_time;
    float frame_max_time;
    float frame_avg_time;
    double run_time;
    size_t n_frames_sampled;

    LONGLONG qpc_start_time;
    LONGLONG qpc_prev_time;
    LONGLONG qpc_end_time;
    double qpc_freq;
} PerfCounter;

void perf_counter_init(PerfCounter *pc);
void perf_counter_frame_update(PerfCounter *pc);
void perf_counter_end(PerfCounter *pc);
void perf_counter_output(PerfCounter *pc, char *buff, size_t len);