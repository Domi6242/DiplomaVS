#include "perf_tracker.h"

double perfTimeDiffMs(clock_t start_ticks, clock_t stop_ticks) {
    return (double)(stop_ticks - start_ticks) * 1000.0 / (double)CLOCKS_PER_SEC;
}

clock_t _perfStartTick, _perfEndTick;
static double _perfTotalTimeMs;
unsigned long _perfTotalCount;

void perfInit() {
    _perfTotalTimeMs = 0;
    _perfTotalCount = 0;
}

void perfStart() {
    _perfStartTick = clock();
}

void perfStop() {
    _perfEndTick = clock();

    _perfTotalTimeMs += perfTimeDiffMs(_perfStartTick, _perfEndTick);
    _perfTotalCount++;
}

double perfGetTotalTimeMs() {
    return _perfTotalTimeMs;
}

unsigned long perfGetTotalCount() {
    return _perfTotalCount;
}

double perfGetAverageTimeMs() {
    return _perfTotalTimeMs / (double)_perfTotalCount;
}