#include "Renderer.h"
/*
												WARNING
	This whole file needs refactoring, this is currently a spike implemnetation of a D3D12 Renderer
									The code is therefore very messy.
 */

// TODO create gloabl throw if failed function

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

HRESULT Renderer::Initialize()
{

	bool bResult = true;

	ComPtr<ID3D12Debug5> debugController;

	//add debug flags
	if (bResult &= SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(true);
		debugController->SetEnableAutoName(true);
		std::cout << "D3D12 Debug Layer Has Been Enabled" << std::endl;
	}

	// create dxgi factory
	ComPtr<IDXGIFactory7> factory;
	DWORD dxgiFactoryFlags = 0;
	dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	bResult &= SUCCEEDED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

	ComPtr<ID3D12Device> device;
	ComPtr<IDXGIAdapter4> adapter;
	for (uint32_t id = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapterByGpuPreference(id, DXGI_GPU_PREFERENCE_UNSPECIFIED, IID_PPV_ARGS(&adapter)); ++id)
	{
		// Contains description of the graphics hardware adapter (GPU)
		DXGI_ADAPTER_DESC3 desc;
		adapter->GetDesc3(&desc);

		// Device that supports D3D12 is found
		if (bResult &= SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&device))))
		{
			break;
		}
	}

	/* TODO: Info Queue

	ComPtr<ID3D12InfoQueue1> infoQueue;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
	{

	}
	*/

	// create command queue
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ComPtr<ID3D12CommandQueue> commandQueue;
	bResult &= SUCCEEDED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue))); // TODO: command list manager

	// create swap chain
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = 3; // Triple Buffering!
	swapChainDesc.Width = 800; // interface with the window size
	swapChainDesc.Height = 800;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	// TODO: Define all descs

	ComPtr<IDXGISwapChain1> swapChain;

	// TODO: Add fullscreen support
	bResult &= SUCCEEDED(factory->CreateSwapChainForHwnd(commandQueue.Get(), WindowsApplication::GetHwnd(), &swapChainDesc, nullptr, nullptr, &swapChain));	// use CreateSwapChainForCoreWindow for Windows Store apps

	bResult &= SUCCEEDED(factory->MakeWindowAssociation(WindowsApplication::GetHwnd(), DXGI_MWA_NO_ALT_ENTER)); // disables full screen
	
	// TODO: follow d3d12 Samples member variables
	ComPtr<IDXGISwapChain4> newSwap;
	swapChain.As(&newSwap); // seems like you can only create a swapchain1. Therefore to get currentBackbuffer

	UINT frameIndex = newSwap->GetCurrentBackBufferIndex(); // DirectX's "MiniEngine" does not do this?

	// create descriptor heaps
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = 3;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // create render target descriptor heap.
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	
	ComPtr<ID3D12DescriptorHeap> rtvHeap;

	bResult &= SUCCEEDED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)));
	UINT descriptorHeapSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());

	ComPtr<ID3D12Resource2> renderTargets[3];
	for (UINT n = 0; n < 3; ++n)
	{
		bResult &= SUCCEEDED(newSwap->GetBuffer(n, IID_PPV_ARGS(&renderTargets[n])));
		device->CreateRenderTargetView(renderTargets[n].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, descriptorHeapSize);
	}
	ComPtr<ID3D12CommandAllocator> commandAllocator;

	bResult &= SUCCEEDED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)));

	return 0;
}
