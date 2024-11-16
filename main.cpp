// main.cpp

#include <windows.h>
#include "Window.h" // Include the Window header file

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nShowCmd) {
    // Create an instance of the Window class
    Window mainWindow(hInstance, 1000, 800);

    // Initialize the window
    if (!mainWindow.Initialize(nShowCmd)) {
        MessageBox(nullptr, L"Failed to initialize window!", L"Error", MB_OK);
        return -1;
    }

    // Run the message loop
    return mainWindow.Run();
}
