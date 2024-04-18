#pragma once
#include <iostream>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dx12/d3dx12.h> // include d3d
#include <wrl.h>
#include "WindowsApplication.h"
using namespace Microsoft::WRL;

namespace RendererPrivate
{
	constexpr uint8_t MAX_FRAMES = 3;
}

class Renderer
{
public:
	Renderer();
	~Renderer();
	bool Initialize();
	void GetDevice();
private:
	ComPtr<ID3D12Device> m_device;
	ComPtr<IDXGIAdapter4> m_adapter;

	ComPtr<IDXGISwapChain4> m_swapChain;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12Resource2> m_renderTargets[RendererPrivate::MAX_FRAMES];
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	UINT frameIndex;
};

