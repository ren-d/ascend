#include <iostream>
#include <d3d12.h>
#include <wrl.h>
#include "WindowsApplication.h"
using namespace Microsoft::WRL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{

	ComPtr<ID3D12Debug> DebugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController))))
	{
		DebugController->EnableDebugLayer();
		std::cout << "D3D12 Debug Layer Has Been Enabled" << std::endl;
	}

	std::cout << "ascend." << std::endl;
	return 0;
}