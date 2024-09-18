#include "test.h"
#include "globals.h"

#include <sysinfoapi.h>
#include <memory.h>
#include <strsafe.h>
#include <time.h>
#include <corecrt.h>
#include <windows.h>
#include <fileapi.h>
#include <assert.h>
#include <psapi.h>


#define LOG_BUFF_LEN 4096

Test test_init() {
    Test test = {0};

    test.current_task = TEST_NONE;
    test.task_start_ms = 0;
    test.task_frame_count = 0;
    test.task_total_time = 0.0f;
    test.task_mem_sum = 0.0f;
    memset(test.avg_times_ms, 0, sizeof(test.avg_times_ms));
    memset(test.slowest_frame_ms, 0, sizeof(test.slowest_frame_ms));
    memset(test.avg_mem_usage, 0, sizeof(test.avg_mem_usage));

    return test;
}

static void write_to_file(Test *test) {
    CreateDirectoryW(L"logs", NULL);

    // prepare file
    time_t currTime = time(NULL);
    struct tm formated_time = *localtime(&currTime);
    WCHAR path_buffer[32] = { 0 };
    StringCchPrintfW(path_buffer, 32, L"logs\\%d-%02d-%02d %02d-%02d.txt", formated_time.tm_year + 1900, formated_time.tm_mon + 1, formated_time.tm_mday, formated_time.tm_hour, formated_time.tm_min);

    // create file
    HANDLE file_handle = CreateFile(path_buffer,               // name of the write
        GENERIC_WRITE,          // open for writing
        0,                      // do not share
        NULL,                   // default security
        CREATE_ALWAYS,          // create new file only
        FILE_ATTRIBUTE_NORMAL,  // normal file
        NULL);                  // no attr. template

    if (file_handle == INVALID_HANDLE_VALUE) {
        assert(0 == "Unable to open file for write.");
    }

    // write data to buffer
    WCHAR buffer[LOG_BUFF_LEN] = { 0 };

#ifdef DOMI_CAIRO
    StringCchPrintf(buffer, LOG_BUFF_LEN, L"Cairo");
#elif DOMI_DIRECT2D
    StringCchPrintf(buffer, LOG_BUFF_LEN, L"Direct2d");
#elif DOMI_RAYLIB
    StringCchPrintf(buffer, LOG_BUFF_LEN, L"raylib");
#endif // DOMI_CAIRO

    StringCchPrintf(buffer, LOG_BUFF_LEN, L"%s - %d-%02d-%02d %02d-%02d\nTest name,Avg frame time,Max frame time,Avg memory usage\n", buffer, formated_time.tm_year + 1900, formated_time.tm_mon + 1, formated_time.tm_mday, formated_time.tm_hour, formated_time.tm_min);

    for (int i = 1; i < TEST_COUNT; i++) {
        StringCchPrintf(buffer, LOG_BUFF_LEN, L"%s%s,%fms,%fms,%fMB\n", buffer, testName[i], test->avg_times_ms[i], test->slowest_frame_ms[i], test->avg_mem_usage[i]);
    }

    size_t bytes_to_write;
    StringCbLength(buffer, LOG_BUFF_LEN, &bytes_to_write);

    BOOL success = WriteFile(
        file_handle,           // open file handle
        buffer,            // start of data to write
        (DWORD)bytes_to_write,  // number of bytes to write
        NULL,        // number of bytes that were written
        NULL
    );

    if (!success) {
        assert(0 == "Failed to write to file.");
    }

    success = CloseHandle(file_handle);

    if (!success) {
        assert(0 == "Failed to close file.");
    }
}

int test_update(Test *test, float delta) {
    // First call is before the first frame
    if (test->current_task == TEST_NONE) {
        test->current_task = 1;
        return 0;
    }

    // Frame count and time
    test->task_frame_count++;
    test->task_total_time += delta;

    // slowest frame update
    if (test->slowest_frame_ms[test->current_task] < delta * 1000.0f) {
        test->slowest_frame_ms[test->current_task] = delta * 1000.0f;
    }

    // memory usage
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc));
    size_t cur_mem_used = pmc.PrivateUsage;
    test->task_mem_sum += (float)cur_mem_used / 1000000.0f; //1048576.0f;
    //test->avg_mem_usage[test->current_task] = test->avg_mem_usage[test->current_task] + ((float)cur_mem_used - test->avg_mem_usage[test->current_task]) / (float)test->task_frame_count;


    // GetTickCount64 returns ms from system start
    int task_time_out = GetTickCount64() - test->task_start_ms > TEST_TIME_MS ? 1 : 0;


    // Task time out -> save data and next task
    if (task_time_out == 0) {
        return 0;
    }

    // calc and save results
    float avg_frame_time_ms = 1000 * (test->task_total_time / (float)test->task_frame_count);
    test->avg_times_ms[test->current_task] = avg_frame_time_ms;
    test->avg_mem_usage[test->current_task] = test->task_mem_sum / (float)test->task_frame_count;

    // Handle next task switch
    test->current_task++;
    test->task_start_ms = GetTickCount64();

    // finalise test
    if (test->current_task == TEST_COUNT) {
        write_to_file(test);
        *test = test_init();
        return -1;
    }

    return 0;
}