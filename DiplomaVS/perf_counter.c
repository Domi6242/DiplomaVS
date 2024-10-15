#include "perf_counter.h"
#include <float.h>
#include <Windows.h>
#include <profileapi.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

void perf_counter_init(PerfCounter *pc) {
    pc->frame_min_time = FLT_MAX;
    pc->frame_max_time = 0.0f;
    pc->frame_avg_time = 0.0f;
    pc->n_frames_sampled = 0;
    pc->run_time = 0.0;
    pc->qpc_start_time = 0;
    pc->qpc_prev_time = 0;
    pc->qpc_end_time = 0;
    pc->qpc_freq = 0.0;

    LARGE_INTEGER li;
    if (!QueryPerformanceFrequency(&li)) {
        assert(NULL == "QueryPerformanceFrequency() Failed!");
    }

    pc->qpc_freq = (double)li.QuadPart;
    QueryPerformanceCounter(&li);
    pc->qpc_start_time = li.QuadPart;
    pc->qpc_prev_time = pc->qpc_start_time;
}

void perf_counter_frame_update(PerfCounter *pc) {
    // get frame time in seconds
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    double elapsed_time = (double)(li.QuadPart - pc->qpc_prev_time) / pc->qpc_freq;

    // update variables
    if (elapsed_time < pc->frame_min_time) {
        pc->frame_min_time = elapsed_time;
    }
    if (elapsed_time > pc->frame_max_time) {
        pc->frame_max_time = elapsed_time;
    }
    pc->n_frames_sampled++;

    // update prev time, for next call
    pc->qpc_prev_time = li.QuadPart;
}

void perf_counter_end(PerfCounter *pc) {
    // Get total time
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    double elapsed_time = (double)(li.QuadPart - pc->qpc_start_time) / pc->qpc_freq;

    pc->run_time = elapsed_time;
    pc->frame_avg_time = pc->run_time / (double)pc->n_frames_sampled;
}

void perf_counter_output(PerfCounter *pc, char *buff, size_t len) {
    int n = snprintf(buff, len, "\n\
        Sampled frames:\t\t%u \n\
        Run time\t\t\t%fs\n\
        Average frame time:\t\t%fs\n\
        Min frame time:\t\t%fs\n\
        Max frame time:\t\t%fs\n",
        pc->n_frames_sampled, pc->run_time, pc->frame_avg_time, pc->frame_min_time, pc->frame_max_time);

    if (n < 0) {
        assert(NULL == "Failed output performace string");
    }
}