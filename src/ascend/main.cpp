#include <iostream>
#include <d3d12.h>
#include <wrl.h>
#include "WindowsApplication.h"
#include "Renderer.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	WindowsApplication::Run(hInstance, nCmdShow);
	std::cout << "ascend." << std::endl;
	return 0;
}