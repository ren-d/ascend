#include <iostream>
#include <d3d12.h>
#include <dxgi1_6.h>
 // include d3d
#include <wrl.h>
using namespace Microsoft::WRL;

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


namespace RendererPrivate
{
	constexpr uint8_t MAX_FRAMES = 3;
}

class Renderer
{
public:
	Renderer() {};
	~Renderer() {};
	bool Initialize()
	{
		bool bResult = true;
		DWORD dxgiFactoryFlags = 0;
		//add debug flags
		if (bResult &= SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_debugController))))
		{
			m_debugController->EnableDebugLayer();
			m_debugController->SetEnableGPUBasedValidation(true);
			//m_debugController->SetEnableAutoName(true);
			std::cout << "D3D12 Debug Layer Has Been Enabled" << std::endl;
		}

		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		// create dxgi factory
		ComPtr<IDXGIFactory7> m_factory;
		bResult &= SUCCEEDED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)));


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

		bResult &= SUCCEEDED(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue))); // TODO: command list manager

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
		bResult &= SUCCEEDED(m_factory->CreateSwapChainForHwnd(m_commandQueue.Get(), WindowsApplication::GetHwnd(), &swapChainDesc, nullptr, nullptr, &tempSwapChain));	// use CreateSwapChainForCoreWindow for Windows Store apps

		bResult &= SUCCEEDED(m_factory->MakeWindowAssociation(WindowsApplication::GetHwnd(), DXGI_MWA_NO_ALT_ENTER)); // disables full screen

		// TODO: follow d3d12 Samples member variables
		bResult &= SUCCEEDED(tempSwapChain.As(&m_swapChain)); // seems like you can only create a swapchain1. Therefore to get currentBackbuffer

		UINT frameIndex = m_swapChain->GetCurrentBackBufferIndex(); // DirectX's "MiniEngine" does not do this?

		// create descriptor heaps
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = 3;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // create render target descriptor heap.
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		bResult &= SUCCEEDED(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));
		UINT descriptorHeapSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		//CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		ComPtr<ID3D12Resource2> renderTargets[3];
		for (UINT n = 0; n < RendererPrivate::MAX_FRAMES; ++n)
		{
			bResult &= SUCCEEDED(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
			//m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, nullptr);
			//rtvHandle.Offset(1, descriptorHeapSize);
		}
		ComPtr<ID3D12CommandAllocator> commandAllocator;

		bResult &= SUCCEEDED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)));

		return bResult;
	}
	bool InitializeDebugLayer()
	{
		return SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_debugController)));
	}
	void GetDevice() {};
private:
	ComPtr<ID3D12Device> m_device;
	ComPtr<IDXGIAdapter4> m_adapter;
	ComPtr<ID3D12Debug1> m_debugController;
	ComPtr<IDXGISwapChain4> m_swapChain;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12Resource2> m_renderTargets[RendererPrivate::MAX_FRAMES];
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	UINT frameIndex;
};
#include <Windows.h>



#include "WinUser.h"
HWND WindowsApplication::m_hwnd = nullptr;
int WindowsApplication::Run(HINSTANCE hInstance, int nCmdShow)
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
	m_hwnd = CreateWindow(
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
		nullptr);

	ShowWindow(m_hwnd, nCmdShow);

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

	// Return this part of the WM_QUIT message to Windows.
	return static_cast<char>(msg.wParam);;
}

LRESULT CALLBACK WindowsApplication::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	// Handle any messages the switch statement didn't.
	return DefWindowProc(hWnd, message, wParam, lParam);
}