#include "ascendCore.h"
//#include "WindowsApplication.h"
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int Run(HINSTANCE hInstance, int nCmdShow, ascend* ascend_);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	std::cout << "ascend." << std::endl;
    ascend ascend_2;
    Run(hInstance, nCmdShow, &ascend_2);
	return 0;
}

int Run(HINSTANCE hInstance, int nCmdShow, ascend* ascend_)
{
    WNDCLASSEX windowClass = { 0 };
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = L"ascendClass";
    RegisterClassEx(&windowClass);

    RECT windowRect = { 0, 0, 800, 800 };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    // Create the window and store a handle to it.
    ascend_->hwnd = CreateWindow(
        windowClass.lpszClassName,
        L"ascend.editor",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,        // We have no parent window.
        nullptr,        // We aren't using menus.
        hInstance,
        ascend_);

    ascend_->Initialize();
    ShowWindow(ascend_->hwnd, nCmdShow);

    // Main
        // Main sample loop.
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        // Process any messages in the queue.
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    ascend_->OnDestroy();
    // Return this part of the WM_QUIT message to Windows.
    return static_cast<char>(msg.wParam);;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ascend* ascend_ = reinterpret_cast<ascend*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    switch (message)
    {
    case WM_CREATE:
    {
        // Save the DXSample* passed in to CreateWindow.
        LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
    }
    return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        ascend_->OnRender();
        return 0;
    }

    // Handle any messages the switch statement didn't.
    return DefWindowProc(hWnd, message, wParam, lParam);
}