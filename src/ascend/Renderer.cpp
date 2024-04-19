#include "Renderer.h"
/*
												WARNING
	This whole file needs refactoring, this is currently a spike implemnetation of a D3D12 Renderer
									The code is therefore very messy.
		Most of this code is adapted from https://github.com/microsoft/DirectX-Graphics-Samples
 */

void VerifyD3D12Result(HRESULT D3DResult, const char* code, const char* Filename, INT32 Line)
{
	if (FAILED(D3DResult))
	{
		char message[60];
		sprintf(message, "D3D12 ERROR: %s failed at %s:%u\nWith the ERROR %08X \n", code, Filename, Line, (INT32)D3DResult);
		OutputDebugStringA(message);
		exit(0);
	}
}

#define VERIFYD3D12RESULT(x) \
{ \
	HRESULT hr = x; \
	if(FAILED(hr)) \
	{ \
		VerifyD3D12Result(hr, #x, __FILE__, __LINE__); \
	} \
}\

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

bool Renderer::Initialize()
{
	bool bResult = true;
	DWORD dxgiFactoryFlags = 0;
	ComPtr<ID3D12Debug1> m_debugController;
#if DEBUG
	//add debug flags
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_debugController))))
	{
		m_debugController->EnableDebugLayer();
		m_debugController->SetEnableGPUBasedValidation(true);
		//m_debugController->SetEnableAutoName(true);   <-- ID3D12Debug4 method
		std::cout << "D3D12 Debug Layer Has Been Enabled" << std::endl;
	}
	
	dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
	// create dxgi factory
	ComPtr<IDXGIFactory7> m_factory;
	VERIFYD3D12RESULT(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)));

	
	for (uint32_t id = 0; DXGI_ERROR_NOT_FOUND != m_factory->EnumAdapterByGpuPreference(id, DXGI_GPU_PREFERENCE_UNSPECIFIED, IID_PPV_ARGS(&m_adapter)); ++id)
	{
		// Contains description of the graphics hardware adapter (GPU)
		DXGI_ADAPTER_DESC3 desc;
		m_adapter->GetDesc3(&desc);

		// Device that supports D3D12 is found
		if (bResult &= SUCCEEDED(D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device))))
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

	VERIFYD3D12RESULT(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue))); // TODO: command list manager

	// create swap chain
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = RendererPrivate::MAX_FRAMES;
	swapChainDesc.Width = 800;
	swapChainDesc.Height = 800;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	// TODO: Define all descs

	ComPtr<IDXGISwapChain1> tempSwapChain;

	// TODO: Add fullscreen support
	VERIFYD3D12RESULT(m_factory->CreateSwapChainForHwnd(m_commandQueue.Get(), WindowsApplication::GetHwnd(), &swapChainDesc, nullptr, nullptr, &tempSwapChain));	// use CreateSwapChainForCoreWindow for Windows Store apps

	VERIFYD3D12RESULT(m_factory->MakeWindowAssociation(WindowsApplication::GetHwnd(), DXGI_MWA_NO_ALT_ENTER)); // disables full screen
	
	// TODO: follow d3d12 Samples member variables
	VERIFYD3D12RESULT(tempSwapChain.As(&m_swapChain)); // seems like you can only create a swapchain1. Therefore to get currentBackbuffer

	UINT frameIndex = m_swapChain->GetCurrentBackBufferIndex(); // DirectX's "MiniEngine" does not do this?

	// create descriptor heaps
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = 3;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // create render target descriptor heap.
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	VERIFYD3D12RESULT(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));
	UINT descriptorHeapSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

	ComPtr<ID3D12Resource2> renderTargets[3];
	for (UINT n = 0; n < RendererPrivate::MAX_FRAMES; ++n)
	{
		VERIFYD3D12RESULT(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
		m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, descriptorHeapSize);
	}
	ComPtr<ID3D12CommandAllocator> commandAllocator;

	VERIFYD3D12RESULT(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)));

	return bResult;
}
