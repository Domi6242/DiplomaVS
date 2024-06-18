#pragma once
#include <Windows.h>

static LPCWSTR logPath = L"logs";

BOOL logInit(LPWSTR subjectPath);
BOOL logWriteStr(LPWSTR str);
BOOL logClose();

void logWriteLogSection(LPWSTR sectionTitle);