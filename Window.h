// Window.h

#pragma once

#include <windows.h>
#include "Graphics.h"
#include <unordered_map>
#include "Player.h"
#include "Camera.h"


class Window {
public:
    Window(HINSTANCE hInstance, int width, int height);
    ~Window();

    bool Initialize(int nShowCmd);
    int Run();

    DirectX::XMMATRIX GetPlayerViewMatrix() const;

    std::unordered_map<WPARAM, bool> keyStates;
    Camera camera;
    static Player* player;
    int width = 1000;
    int height = 800;

private:
    HWND hwnd = nullptr;
    HINSTANCE hInstance = nullptr;
    LPCWSTR windowClassName = L"WindowClass";
    LPCWSTR windowTitle = L"BogEngine";

    Graphics graphics;

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    bool mIgnoreNextMouseMove;
    bool mCursorLocked = true;

    void LockCursor() {
        if (mCursorLocked) {
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            ClientToScreen(hwnd, (POINT*)&clientRect.left);
            ClientToScreen(hwnd, (POINT*)&clientRect.right);
            ClipCursor(&clientRect);
            ShowCursor(FALSE);
        }
        else {
            ClipCursor(NULL);
            ShowCursor(TRUE);
        }
    }

};
