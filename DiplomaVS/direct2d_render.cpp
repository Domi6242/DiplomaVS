#ifdef DOMI_DIRECT2D
#include "direct2d_render.h"

#include <Windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#include <cmath>

extern "C" {
#include "delta_time.h"
#include "globals.h"
#include "shape_generator.h"
#include "test.h"
#include <intsafe.h>
#include <WTypesbase.h>
#include <combaseapi.h>
#include <dcommon.h>
#include <Unknwnbase.h>
#include <basetsd.h>
}

void releaseD2D1Window(D2dObj *rt) {
    SafeRelease(&rt->pD2dFactory);
    SafeRelease(&rt->pRenderTarget);
    SafeRelease(&rt->pFillBrush);
    SafeRelease(&rt->pBorderBrush);
}

HRESULT createDeviceIndependentResources(D2dObj *rt) {
    HRESULT hr = S_OK;
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &rt->pD2dFactory);

    if (SUCCEEDED(hr)) {
        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown **>(&rt->pDWriteFactory)
        );
    }



    if (SUCCEEDED(hr)) {
        hr = rt->pDWriteFactory->CreateTextFormat(
            L"Iosevka Etoile",  // Font family name.
            NULL,       // Font collection (NULL sets it to use the system font
            // collection).
            DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, TEXT_SIZE, L"en-us", &rt->pTextFormat
        );
    }

    // Create a text layout using the text format.
    size_t str_len = lstrlenW(testTextW);
    rt->str = testTextW;
    if (SUCCEEDED(hr)) {
        hr = rt->pDWriteFactory->CreateTextLayout(
            testTextW,           // The string to be laid out and formatted.
            (UINT32)str_len,      // The length of the string.
            rt->pTextFormat,     // The text format to apply to the string (contains
            // font information, etc).
            WINDOW_WIDTH * 10,   // The width of the layout box.
            WINDOW_HEIGHT * 10,  // The height of the layout box.
            &rt->pTextLayout     // The IDWriteTextLayout interface pointer.
        );
    }

    return hr;
}

HRESULT createDeviceResources(D2dObj *rt) {
    HRESULT hr = S_OK;

    if (rt->pRenderTarget) {
        return hr;
    }

    rt->imageAngleRad = IMAGE_ANGLE_RAD;
    rt->imageAngleSpeed = IMAGE_ROTATION_SPEED_RAD;
    rt->imageScale = IMAGE_SCALE;
    rt->imageScaleSpeed = IMAGE_SCALE_SPEED;

    rt->textSize = TEXT_SIZE;
    rt->textScaleSpeed = TEXT_SCALE_SPEED;

    rt->running_test = 0;

    // get window size
    RECT rc;
    GetClientRect(rt->m_hwnd, &rc);
    D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

    // Create a Direct2D render target.
    hr = rt->pD2dFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(rt->m_hwnd, size), &rt->pRenderTarget
    );

    // Create Brushes
    if (SUCCEEDED(hr)) {
        hr = rt->pRenderTarget->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::White), &rt->pFillBrush
        );
    }
    if (SUCCEEDED(hr)) {
        hr = rt->pRenderTarget->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Black), &rt->pBorderBrush
        );
    }
    if (SUCCEEDED(hr)) {
        hr = rt->pRenderTarget->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Black), &rt->pTextBrush
        );
    }

    rt->test = test_init();

    // Start frame delta timer
    if (SUCCEEDED(hr)) {
        deltaInit();
        deltaUpdate();
    }

    // Load image
    if (SUCCEEDED(hr)) {
        hr = loadImageResource(rt);
    }

    return hr;
}

HRESULT loadImageResource(D2dObj *rt) {
    IWICImagingFactory *wicFactory = NULL;
    IWICBitmapDecoder *pDecoder = NULL;
    IWICBitmapFrameDecode *pSource = NULL;
    IWICStream *pStream = NULL;
    IWICFormatConverter *pConverter = NULL;
    IWICBitmapScaler *pScaler = NULL;

    HRESULT hr =
        CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID *)&wicFactory);

    if (SUCCEEDED(hr)) {
        hr = wicFactory->CreateDecoderFromFilename(
            testImagePathW, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad,
            &pDecoder
        );
    }

    // Retrieve the first bitmap frame.
    if (SUCCEEDED(hr)) {
        hr = pDecoder->GetFrame(0, &pSource);
    }

    // Create the scaler.
    if (SUCCEEDED(hr)) {
        hr = wicFactory->CreateBitmapScaler(&pScaler);
    }

    // Scale the source image to fit the screen
    if (SUCCEEDED(hr)) {
        UINT src_w, src_h;
        pSource->GetSize(&src_w, &src_h);

        double ratio_x = (double)WINDOW_WIDTH / (double)src_w;
        double ratio_y = (double)WINDOW_HEIGHT / (double)src_h;
        double scale_factor = ratio_x < ratio_y ? ratio_x : ratio_y;
        hr = pScaler->Initialize(pSource, src_w * scale_factor, src_h * scale_factor, WICBitmapInterpolationModeFant);
    }

    if (SUCCEEDED(hr)) {
        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        hr = wicFactory->CreateFormatConverter(&pConverter);
    }

    if (SUCCEEDED(hr)) {
        hr = pConverter->Initialize(pScaler, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeCustom);
    }

    if (SUCCEEDED(hr)) {
        // Create a Direct2D bitmap from the WIC bitmap.
        hr = rt->pRenderTarget->CreateBitmapFromWicBitmap(pConverter, NULL, &rt->testImage);
    }

    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pStream);
    SafeRelease(&pConverter);
    SafeRelease(&pScaler);

    return hr;
}

void discardDeviceResources(D2dObj *rt) {
    SafeRelease(&rt->pRenderTarget);
    SafeRelease(&rt->pFillBrush);
    SafeRelease(&rt->pBorderBrush);
    SafeRelease(&rt->pTextBrush);
}

static void draw_rect(D2dObj *rt, ShapeRect rect) {
    D2D1_RECT_F d2dRect = D2D1::RectF(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);

    rt->pFillBrush->SetColor(D2D1::ColorF(rect.fill.r, rect.fill.g, rect.fill.b, rect.fill.a));
    rt->pBorderBrush->SetColor(D2D1::ColorF(rect.border.r, rect.border.g, rect.border.b, rect.border.a));

    rt->pRenderTarget->FillRectangle(&d2dRect, rt->pFillBrush);
    rt->pRenderTarget->DrawRectangle(&d2dRect, rt->pBorderBrush);
}

static void draw_circle(D2dObj *rt, ShapeCircle circle) {
    D2D1_ELLIPSE d2dEllipse = D2D1::Ellipse(D2D1::Point2F(circle.x, circle.y), circle.rad, circle.rad);

    rt->pFillBrush->SetColor(D2D1::ColorF(circle.fill.r, circle.fill.g, circle.fill.b, circle.fill.a));
    rt->pBorderBrush->SetColor(D2D1::ColorF(circle.border.r, circle.border.g, circle.border.b, circle.border.a));

    rt->pRenderTarget->FillEllipse(&d2dEllipse, rt->pFillBrush);
    rt->pRenderTarget->DrawEllipse(&d2dEllipse, rt->pBorderBrush);
}

static void draw_triangle(D2dObj *rt, ShapeTriangle triangle) {
    HRESULT hr = S_OK;

    ID2D1GeometrySink *pSink = NULL;
    ID2D1PathGeometry *d2dTriangle;

    hr = rt->pD2dFactory->CreatePathGeometry(&d2dTriangle);

    if (SUCCEEDED(hr)) {
        hr = d2dTriangle->Open(&pSink);
    }

    if (SUCCEEDED(hr)) {
        pSink->SetFillMode(D2D1_FILL_MODE_WINDING);
        pSink->BeginFigure(D2D1::Point2F(triangle.x1, triangle.y1), D2D1_FIGURE_BEGIN_FILLED);
        pSink->AddLine(D2D1::Point2F(triangle.x2, triangle.y2));
        pSink->AddLine(D2D1::Point2F(triangle.x3, triangle.y3));
        pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
        hr = pSink->Close();
    }

    if (SUCCEEDED(hr)) {
        SafeRelease(&pSink);

        rt->pFillBrush->SetColor(D2D1::ColorF(triangle.fill.r, triangle.fill.g, triangle.fill.b, triangle.fill.a));
        rt->pBorderBrush->SetColor(D2D1::ColorF(triangle.border.r, triangle.border.g, triangle.border.b, triangle.border.a));

        rt->pRenderTarget->FillGeometry(d2dTriangle, rt->pFillBrush);
        rt->pRenderTarget->DrawGeometry(d2dTriangle, rt->pBorderBrush);

        SafeRelease(&d2dTriangle);
    }
}

static void test_shapes(D2dObj *rt, int shape_num) {
    // Loop through all the shapes
    for (int i = 0; i < shape_num; i++) {
        switch (i % 3) {
        case 0:
            draw_rect(rt, generateRectangle());
            break;
        case 1:
            draw_circle(rt, generateCircle());
            break;
        case 2:
            draw_triangle(rt, generateTriangle());
            break;
        default:
            break;
        }
    }
}

static void test_image(D2dObj *rt, float delta) {
    if (rt->testImage == NULL) {
        // skip if image has not yet been processed by direct2d
        return;
    }

    // flip scaling direction
    if ((rt->imageScale > IMAGE_SCALE_MAX && rt->imageScaleSpeed > 0) ||
        (rt->imageScale < IMAGE_SCALE_MIN && rt->imageScaleSpeed < 0)) {
        rt->imageScaleSpeed = -rt->imageScaleSpeed;
    }

    // smoothly transform image with delta time
    rt->imageScale += rt->imageScaleSpeed * delta;
    rt->imageAngleRad += rt->imageAngleSpeed * delta;

    // calculate the scaled size
    D2D1_SIZE_F imageSize = rt->testImage->GetSize();
    D2D1_POINT_2F scaledSize = D2D1::Point2F((imageSize.width) * rt->imageScale, (imageSize.height) * rt->imageScale);

    // calcultae center of screen
    D2D1_POINT_2F imageCenter = D2D1::Point2F(WINDOW_WIDTH / 2.0f,  // test
        WINDOW_HEIGHT / 2.0f  // test
    );

    // calculate upper left origin of image
    D2D1_POINT_2F upperLeftCorner = D2D1::Point2F(
        imageCenter.x - scaledSize.x / 2.0f, imageCenter.y - scaledSize.y / 2.0f
    );

    // rotate the image around it's center
    rt->pRenderTarget->SetTransform(
        D2D1::Matrix3x2F::Rotation(RADDEG(rt->imageAngleRad), imageCenter)
    );

    // draw the image to the screen
    rt->pRenderTarget->DrawBitmap(
        rt->testImage, D2D1::RectF(upperLeftCorner.x, upperLeftCorner.y, upperLeftCorner.x + scaledSize.x, upperLeftCorner.y + scaledSize.y)
    );
}

static void draw_text(D2dObj *rt, LPCWSTR str, float x, float y, float font_size) {
    size_t str_len = lstrlenW(str);

    // To update text we have to recreate the text layout
    if (lstrcmpW(str, rt->str)) {
        rt->str = str;

        SafeRelease(&rt->pTextLayout);
        rt->pDWriteFactory->CreateTextLayout(
            rt->str,                     // The string to be laid out and formatted.
            str_len,                     // The length of the string.
            rt->pTextFormat,             // The text format to apply to the string (contains
            // font information, etc).
            WINDOW_WIDTH * 10,           // The width of the layout box.
            WINDOW_HEIGHT * 10,          // The height of the layout box.
            &rt->pTextLayout             // The IDWriteTextLayout interface pointer.
        );
    }

    if (!rt->pTextLayout) { return; } // this should't happen but just in case

    DWRITE_TEXT_RANGE textRange = { 0, str_len };
    rt->pTextLayout->SetFontSize(font_size, textRange);
    rt->pRenderTarget->DrawTextLayout({ x, y }, rt->pTextLayout, rt->pTextBrush);
}

static void test_text(D2dObj *rt, float delta) {
    if ((rt->textSize > TEXT_SIZE_MAX && rt->textScaleSpeed > 0) ||
        (rt->textSize < TEXT_SIZE_MIN && rt->textScaleSpeed < 0)) {
        rt->textScaleSpeed = -rt->textScaleSpeed;
    }
    rt->textSize += rt->textScaleSpeed * delta;

    for (int i = 0; i < TEXT_REPEAT_LINES; i++) {
        draw_text(rt, testTextW, 5.0f, 10.0f + rt->textSize * i, rt->textSize);
    }
}

static void run_tests(D2dObj *rt, float delta) {
    // Initialisation
    if (rt->test.current_task == TEST_NONE) {
        rt->test = test_init();
    }

    // Update
    int test_done = test_update(&rt->test, delta);

    // Finalise
    if (test_done) {
        rt->running_test = 0;
        return;
    }

    // Draw
    switch (rt->test.current_task) {
    case TEST_SHAPES_1:
    case TEST_SHAPES_2:
    case TEST_SHAPES_3:
    case TEST_SHAPES_4:
    case TEST_SHAPES_5:
        test_shapes(rt, shapesXPerFramePerTest[rt->test.current_task]);
        break;
    case TEST_IMAGE:
        test_image(rt, delta);
        break;
    case TEST_TEXT:
        test_text(rt, delta);
        break;
    default:
        break;
    }

    //draw_text(rt, 10.0, 30.0, "Running Tests", 20.0);
}

void renderFrame(D2dObj *rt) {
    HRESULT hr = S_OK;
    hr = createDeviceResources(rt);
    if (!SUCCEEDED(hr)) {
        return;
    }

    // Update and get the delta
    float frameDelta = deltaUpdate();

    // Prepare the window for drawing and clear the screen
    rt->pRenderTarget->BeginDraw();
    rt->pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
    rt->pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
    rt->pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

    switch (rt->running_test) {
    case 0:
        draw_text(rt, L"Use keys 1, 2, 3 to cycle through the tests", 10.0f, 10.0f, 32.0f);
        break;
    case 1:
        test_shapes(rt, 1000);
        break;
    case 2:
        test_image(rt, frameDelta);
        break;
    case 3:
        test_text(rt, frameDelta);
        break;
    case -1:
        run_tests(rt, frameDelta);
        break;
    default:
        break;
    }

    // Finalise drawing
    hr = rt->pRenderTarget->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET) {
        hr = S_OK;
        discardDeviceResources(rt);
    }
}
#endif  // DOMI_DIRECT2D
