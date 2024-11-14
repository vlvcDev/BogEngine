// Window.cpp

#include "Window.h"
#include "resource.h"

Window::Window(HINSTANCE hInstance, int width, int height) : hInstance(hInstance), width(width), height(height) {
}

Window::~Window() {
}

bool Window::Initialize(int nShowCmd) {
    // Register window class
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = Window::WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = windowClassName;
    wc.hIconSm = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    if (!RegisterClassEx(&wc)) {
        MessageBox(nullptr, L"Failed to register window class!", L"Error", MB_OK);
        return false;
    }

    // Create window
    hwnd = CreateWindowEx(
        0,
        windowClassName,
        windowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        nullptr,
        nullptr,
        hInstance,
        this // Pass 'this' pointer to WM_CREATE message
    );

    if (!hwnd) {
        MessageBox(nullptr, L"Failed to create window!", L"Error", MB_OK);
        return false;
    }

    ShowWindow(hwnd, nShowCmd);
    UpdateWindow(hwnd);

    // Initialize Graphics
    if (!graphics.Initialize(hwnd, width, height)) {
        MessageBox(nullptr, L"Failed to initialize graphics!", L"Error", MB_OK);
        return false;
    }

    return true;
}

int Window::Run() {
    MSG msg = {};
    while (true) {
        // Process any messages in the queue
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                return static_cast<int>(msg.wParam);
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Calculate delta time
        static DWORD previousTime = GetTickCount64();
        DWORD currentTime = GetTickCount64();
        float deltaTime = (currentTime - previousTime) / 1000.0f;
        previousTime = currentTime;

        // Update and render
        graphics.Update(deltaTime);
        graphics.Draw();
        graphics.Present();
    }
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Window* pThis = nullptr;

    if (msg == WM_NCCREATE) {
        // Retrieve the 'this' pointer passed in CreateWindowEx via the lpParam parameter
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = reinterpret_cast<Window*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));

        pThis->hwnd = hwnd;
    }
    else {
        // Retrieve pointer to Window instance
        pThis = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (pThis) {
        return pThis->HandleMessage(hwnd, msg, wParam, lParam);
    }
    else {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

LRESULT CALLBACK Window::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) {
            PostQuitMessage(0);
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
