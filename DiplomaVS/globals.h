#pragma once

#ifdef DOMI_RAYLIB
#define NOGDI   // All GDI defines and routines
#define NOUSER  // All USER defines and routines
#endif

#include <Windows.h>  // or any library that uses Windows.h

#ifdef DOMI_RAYLIB  // raylib uses these names as function parameters
#undef near
#undef far
#endif

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Shapes defines
#define SHAPES_MIN_SIZE 20
#define SHAPES_MAX_SIZE 100
// #define SHAPES_USE_ALPHA //  0.907137
#define SHAPES_MIN_ALPHA 0.5f
#define SHAPES_MAX_ALPHA 1.0f

// Image defines
#define IMAGE_ANGLE_RAD 0.0f
#define IMAGE_ROTATION_SPEED_RAD 1.5f
#define IMAGE_SCALE 1.0f
#define IMAGE_SCALE_SPEED 2.0f
#define IMAGE_SCALE_MIN 0.05f
#define IMAGE_SCALE_MAX 2.0f

// Text defines
#define TEXT_SIZE 12.0f
#define TEXT_SCALE_SPEED 100.0f
#define TEXT_SIZE_MIN 8.0f
#define TEXT_SIZE_MAX 72.0f
#define TEXT_REPEAT_LINES 10

// Tests defines
#define TEST_TIME_MS (10 * 1000)

#define M_PI 3.14159265358979323846f
#define RADDEG(x) (x * 57.295779513082320876798154814105f)

typedef unsigned char byte;

static const int shapesXPerFramePerTest[] = { 0, 1000, 5000, 10000, 20000, 50000};
static const int shapesXTestCounts = sizeof(shapesXPerFramePerTest) / sizeof(shapesXPerFramePerTest[0]);

static const char* testImagePath = "C:\\Users\\Dev\\OneDrive - Univerza v Mariboru\\Dokumenti\\DiplomaVS\\media\\fops.png";
static LPCWSTR testImagePathW = L"C:\\Users\\Dev\\OneDrive - Univerza v Mariboru\\Dokumenti\\DiplomaVS\\media\\fops.png";

static const char* testText = "\"The quick brown fox jumps over the lazy dog0123456789\"";
static LPCWSTR testTextW = L"\"The quick brown fox jumps over the lazy dog0123456789\"";
static const char* testFontPath = "C:\\Users\\Dev\\OneDrive - Univerza v Mariboru\\Dokumenti\\DiplomaVS\\media\\IosevkaEtoile-Regular.ttf";
static LPCWSTR testFontPathW = L"C:\\Users\\Dev\\OneDrive - Univerza v Mariboru\\Dokumenti\\DiplomaVS\\media\\IosevkaEtoile-Regular.ttf";