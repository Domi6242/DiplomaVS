#ifdef DOMI_DIRECT2D
#include <Windows.h>

#include <cstdlib>
#include <ctime>

#include "direct2d_render.h"

extern "C" {
#include "globals.h"
#include "test.h"
#include <basetsd.h>
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    D2dObj *window = (D2dObj *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CREATE:
        // initDirect2d(window);
        break;
    case WM_PAINT:
        renderFrame(window);
        ValidateRect(hwnd, NULL);
        break;
    case WM_KEYUP:
        if (window->running_test == -1) {
            break;
        }

        switch (wParam) {
        case '1':
            window->running_test = 1;
            break;
        case '2':
            window->running_test = 2;
            break;
        case '3':
            window->running_test = 3;
            break;
        case '0':
            window->running_test = 0;
            break;
        case ' ':
            window->running_test = -1;
            break;
        case VK_OEM_PLUS:
            window->shapeSelect++;
            if (window->shapeSelect > TEST_SHAPES_5) {
                window->shapeSelect = TEST_SHAPES_5;
            }
            break;
        case VK_OEM_MINUS:
            window->shapeSelect--;
            if (window->shapeSelect < TEST_SHAPES_1) {
                window->shapeSelect = TEST_SHAPES_1;
            }
            break;
        default:
            break;
        }
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

LPCWSTR gMainCLassName = L"Direct2D Example";

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {
    (void)lpCmdLine;
    (void)hPrevInstance;
    srand((unsigned int)time(0));

    // prepare for window initialisation
    WNDCLASSEX wc = {0};
    HWND hwnd;
    HRESULT hr;

    // Initialize device-indpendent resources, such as the Direct2D factory.
    D2dObj d2dWindow = D2dObj{ 0, NULL, NULL, NULL, NULL };
    hr = createDeviceIndependentResources(&d2dWindow);
    // initDirect2d(&d2dWindow);

    if (!SUCCEEDED(hr)) {
        return -1;
    }

    // Step 1: Registering the Window Class
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = gMainCLassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"Window Registration Failed!", L"Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // adjust window size to desired client size
    RECT win_rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    AdjustWindowRectEx(&win_rect, WS_OVERLAPPEDWINDOW, 0, 0);

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE, gMainCLassName, L"Direct2D Test", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, win_rect.right - win_rect.left + 4,
        win_rect.bottom - win_rect.top + 4, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // save the window data to be accesed in the callback throught the window
    // handle
    d2dWindow.m_hwnd = hwnd;
    SetWindowLongPtr(d2dWindow.m_hwnd, GWLP_USERDATA, (LONG_PTR)&d2dWindow);

    // show the window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Step 3: The Message Loop
    MSG msg;
    while (1) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                break;
            } else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        } else {
            InvalidateRect(hwnd, NULL, FALSE);
        }
    }

    return 0;
}
#endif  // DOMI_DIRECT2D
