// Window.cpp

#include "Window.h"
#include "resource.h"
#include <windowsx.h>
#include <iostream>
#include <sstream>
#include <DirectXMath.h>

Player* Window::player = nullptr;

Window::Window(HINSTANCE hInstance, int width, int height)
    : hInstance(hInstance), width(width), height(height), mIgnoreNextMouseMove(false) {
}

Window::~Window() {
    ClipCursor(NULL);  // Release the cursor
    ShowCursor(TRUE);  // Show the cursor
    delete Window::player;
    Window::player = nullptr;
}

void DebugOut(const std::string& msg) {
    std::wstring wMsg(msg.begin(), msg.end());
    OutputDebugString(wMsg.c_str());
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
    SetCapture(hwnd);
    // Hide the cursor
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    ClientToScreen(hwnd, (POINT*)&clientRect.left);
    ClientToScreen(hwnd, (POINT*)&clientRect.right);

    // Confine the cursor to the window
    ClipCursor(&clientRect);

    // Hide the cursor
    ShowCursor(FALSE);


    // Initialize Player
    Window::player = new Player();
    if (!Window::player) {
        MessageBox(nullptr, L"Failed to create player!", L"Error", MB_OK);
        return false;
    }

    // Initialize Graphics
    if (!graphics.Initialize(hwnd, width, height)) {
        MessageBox(nullptr, L"Failed to initialize graphics!", L"Error", MB_OK);
        return false;
    }

    return true;
}

int Window::Run() {
    MSG msg = {};
    LARGE_INTEGER frequency;
    LARGE_INTEGER previousTime;
    LARGE_INTEGER currentTime;
    float deltaTime;

    // Initialize high-resolution timer
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&previousTime);

    while (true) {
        // Process any messages in the queue
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                return static_cast<int>(msg.wParam);
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // In Window::Run()
        const float targetFrameTime = 1.0f / 144.0f; // Target 144 FPS
        const float maxDeltaTime = 0.033f; // Cap at ~30 FPS minimum

        QueryPerformanceCounter(&currentTime);
        deltaTime = static_cast<float>(currentTime.QuadPart - previousTime.QuadPart) / frequency.QuadPart;
        previousTime = currentTime;

        // Clamp deltaTime
        if (deltaTime > maxDeltaTime) {
            deltaTime = maxDeltaTime;
        }

        // Update the player
        if (Window::player) {
            Window::player->Update(deltaTime);
            graphics.SetViewMatrix(Window::player->GetViewMatrix());
        }

        // Update and render graphics
        graphics.Update(deltaTime);
        graphics.Draw();
        graphics.Present();
    }
}

XMMATRIX GetPlayerViewMatrix() {
    return Window::player->GetViewMatrix();
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
    case WM_SIZE:
    case WM_MOVING:
    case WM_MOVE:
    {
        // Update the clip region when the window moves or resizes
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        ClientToScreen(hwnd, (POINT*)&clientRect.left);
        ClientToScreen(hwnd, (POINT*)&clientRect.right);
        ClipCursor(&clientRect);
        return 0;
    }

    case WM_ACTIVATE:
    {
        if (LOWORD(wParam) == WA_INACTIVE) {
            // Window is deactivated, release the cursor
            ClipCursor(NULL);
            ShowCursor(TRUE);
        }
        else {
            // Window is activated, confine cursor and hide it
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            ClientToScreen(hwnd, (POINT*)&clientRect.left);
            ClientToScreen(hwnd, (POINT*)&clientRect.right);
            ClipCursor(&clientRect);
            ShowCursor(FALSE);
        }
        return 0;
    }

    case WM_DESTROY:
    {
        // Release the cursor when the window is destroyed
        ClipCursor(NULL);
        ShowCursor(TRUE);
        PostQuitMessage(0);
        return 0;
    }

    case WM_KEYDOWN:
    {
        if (wParam == VK_TAB) {  // Or any other key you prefer
            mCursorLocked = !mCursorLocked;
            LockCursor();
            return 0;
        }
        if (wParam == VK_ESCAPE) {
            PostQuitMessage(0);
        }
        keyStates[wParam] = true; // Key is pressed
        return 0;
    }

    case WM_MOUSEMOVE:
    {
        if (mIgnoreNextMouseMove) {
            mIgnoreNextMouseMove = false;
            return 0;
        }

        static POINT lastMousePos;
        POINT currentMousePos;
        GetCursorPos(&currentMousePos);

        // Get delta from last position
        int dx = currentMousePos.x - lastMousePos.x;
        int dy = currentMousePos.y - lastMousePos.y;

        if (dx != 0 || dy != 0) {
            // Update player camera with the delta
            if (Window::player) {
                Window::player->OnMouseMove(dx, dy);
            }

            // Store current position for next frame
            lastMousePos = currentMousePos;

            // Recenter the mouse
            POINT center;
            center.x = width / 2;
            center.y = height / 2;
            ClientToScreen(hwnd, &center);

            mIgnoreNextMouseMove = true;
            SetCursorPos(center.x, center.y);
            lastMousePos = center;
        }

        return 0;
    }

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}