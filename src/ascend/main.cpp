#include <iostream>
#include <d3d12.h>
#include <wrl.h>
using namespace Microsoft::WRL;

int main()
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