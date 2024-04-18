#pragma once
#include <Windows.h>
#include "Renderer.h"
/*  Taken from
 *	https://github.com/microsoft/DirectX-Graphics-Samples/tree/master
 */
class WindowsApplication
{
public:
	 static int Run(HINSTANCE hInstance, int nCmdShow);
	 static HWND GetHwnd() { return m_hwnd; }

protected:
	 static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	 static HWND m_hwnd;
};

