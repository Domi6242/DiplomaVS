#include "perf_log.h"

#include <Windows.h>
#include <stdlib.h>
#include <strsafe.h>
#include <time.h>
#include <wchar.h>

#include "perf_tracker.h"

#define LOG_BUFF_LEN 4096

static HANDLE fileHandle;

static WCHAR writeBuffer[LOG_BUFF_LEN];
static size_t writeBufferRemainingSize;

int logInit(LPWSTR subjectPath) {
    wmemset(writeBuffer, 0, LOG_BUFF_LEN);
    writeBufferRemainingSize = LOG_BUFF_LEN;

    time_t currTime = time(NULL);
    struct tm formatedTime = *localtime(&currTime);
    BOOL bErrorFlag = FALSE;

    // construct full date-time string
    WCHAR dateTimeBuff[LOG_BUFF_LEN] = { 0 };
    StringCchPrintfW(dateTimeBuff, LOG_BUFF_LEN, L"%d-%02d-%02d %02d-%02d", formatedTime.tm_year + 1900, formatedTime.tm_mon + 1, formatedTime.tm_mday, formatedTime.tm_hour, formatedTime.tm_min);

    // construct direcotry string for todays folder
    WCHAR direcotryBuff[LOG_BUFF_LEN] = { 0 };
    StringCchPrintfW(direcotryBuff, LOG_BUFF_LEN, L"%s\\%d-%02d-%02d", logPath, formatedTime.tm_year + 1900, formatedTime.tm_mon + 1, formatedTime.tm_mday);

    // create todays direcotry
    CreateDirectoryW(logPath, NULL);
    CreateDirectoryW(direcotryBuff, NULL);

    // get executable name
    size_t pathLen;
    StringCchLengthW(subjectPath, LOG_BUFF_LEN, &pathLen);
    size_t ctr = pathLen - 1;
    while (subjectPath[ctr--] != '\\') {
    }
    ctr += 2;
    size_t exeLen = pathLen - ctr;
    WCHAR exeBuff[LOG_BUFF_LEN] = { 0 };
    StringCchCopyNW(exeBuff, LOG_BUFF_LEN, &subjectPath[ctr], exeLen);

    // construct file path
    WCHAR pathBuff[LOG_BUFF_LEN] = { 0 };
    StringCchPrintfW(pathBuff, LOG_BUFF_LEN, L"%s\\%s %s.txt", direcotryBuff, exeBuff, dateTimeBuff);

    // create file
    fileHandle = CreateFile(pathBuff,               // name of the write
        GENERIC_WRITE,          // open for writing
        0,                      // do not share
        NULL,                   // default security
        CREATE_ALWAYS,          // create new file only
        FILE_ATTRIBUTE_NORMAL,  // normal file
        NULL);                  // no attr. template

    if (fileHandle == INVALID_HANDLE_VALUE) {
        wprintf(L"Terminal failure: Unable to open file for write.\n");
        return TRUE;
    }

    // prepare data to write
    size_t bytesToWrite;
    WCHAR writeBuff[LOG_BUFF_LEN] = { 0 };
    StringCchPrintfW(writeBuff, LOG_BUFF_LEN, L"%s\n%s\n", exeBuff, dateTimeBuff);
    StringCbLengthW(writeBuff, LOG_BUFF_LEN, &bytesToWrite);

    // write to file
    DWORD bytesWritten = 0;
    bErrorFlag = WriteFile(
        fileHandle,           // open file handle
        writeBuff,            // start of data to write
        (DWORD)bytesToWrite,  // number of bytes to write
        &bytesWritten,        // number of bytes that were written
        NULL
    );  // no overlapped structure

    return bErrorFlag;
}

BOOL logWriteStr(LPWSTR str) {
    // prepare data to write
    size_t bytesToWrite;
    WCHAR writeBuff[LOG_BUFF_LEN] = { 0 };
    StringCchPrintfW(writeBuff, LOG_BUFF_LEN, L"%s", str);
    StringCbLengthW(writeBuff, LOG_BUFF_LEN, &bytesToWrite);

    // write to file
    DWORD bytesWritten = 0;
    return WriteFile(
        fileHandle,           // open file handle
        writeBuff,            // start of data to write
        (DWORD)bytesToWrite,  // number of bytes to write
        &bytesWritten,        // number of bytes that were written
        NULL
    );  // no overlapped structure
}

BOOL logClose() {
    return CloseHandle(fileHandle);
}

void logWriteLogSection(LPWSTR sectionTitle) {
    WCHAR direcotryBuff[LOG_BUFF_LEN] = { 0 };
    StringCchPrintf(direcotryBuff, LOG_BUFF_LEN, L"\n%s\nTotal CPU time: %f milliseconds\nRendered frames: %lu\nAverage frame time: %f milliseconds\n", sectionTitle, perfGetTotalTimeMs(), perfGetTotalCount(), perfGetAverageTimeMs());
    logWriteStr(direcotryBuff);
}