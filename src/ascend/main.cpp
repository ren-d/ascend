#include <iostream>
#include <d3d12.h>
#include <wrl.h>
#include "WindowsApplication.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	Renderer renderer;
	WindowsApplication::Run(&renderer, hInstance, nCmdShow);
	std::cout << "ascend." << std::endl;
	return 0;
}