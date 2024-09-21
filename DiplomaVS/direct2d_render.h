#pragma once
#ifdef DOMI_DIRECT2D

extern "C" {
#include "test.h"
}

#include <Windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>

template <class Interface>
inline void SafeRelease(Interface **ppInterfaceToRelease) {
    if (*ppInterfaceToRelease != NULL) {
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = NULL;
    }
}

struct D2dObj {
    HWND m_hwnd;
    ID2D1Factory *pD2dFactory;
    ID2D1HwndRenderTarget *pRenderTarget;
    ID2D1SolidColorBrush *pFillBrush;
    ID2D1SolidColorBrush *pBorderBrush;

    int shapeSelect;

    ID2D1Bitmap *testImage;

    float imageAngleRad;
    float imageAngleSpeed;
    float imageScale;
    float imageScaleSpeed;

    IDWriteFactory *pDWriteFactory;
    IDWriteTextFormat *pTextFormat;
    IDWriteTextLayout *pTextLayout;
    ID2D1SolidColorBrush *pTextBrush;
    LPCWSTR str;
    float textSize;
    float textScaleSpeed;

    Test test;
    int running_test;
};

HRESULT loadImageResource(D2dObj *rt);
HRESULT createDeviceIndependentResources(D2dObj *rt);
HRESULT createDeviceResources(D2dObj *rt);
void discardDeviceResources(D2dObj *rt);
void renderFrame(D2dObj *rt);

#endif  // DOMI_DIRECT2D