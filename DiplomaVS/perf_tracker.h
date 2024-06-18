#pragma once
#include <time.h>

double perfTimeDiffMs(clock_t start_ticks, clock_t stop_ticks);

void perfInit();
void perfStart();
void perfStop();

double perfGetTotalTimeMs();
unsigned long perfGetTotalCount();
double perfGetAverageTimeMs();