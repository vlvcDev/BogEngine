// Window.h

#pragma once

#include <windows.h>
#include "Graphics.h"

class Window {
public:
    Window(HINSTANCE hInstance, int width, int height);
    ~Window();

    bool Initialize(int nShowCmd);
    int Run();

private:
    HWND hwnd = nullptr;
    HINSTANCE hInstance = nullptr;
    int width = 800;
    int height = 600;
    LPCWSTR windowClassName = L"WindowClass";
    LPCWSTR windowTitle = L"DirectX 11 Pyramid";

    Graphics graphics;

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
