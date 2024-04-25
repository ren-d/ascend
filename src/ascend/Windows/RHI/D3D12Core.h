#include <iostream>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dx12/d3dx12.h>
#include <DirectXMath.h>//
/*
	
	ascend. 
			Toy Graphics Renderer

*/
using namespace Microsoft::WRL;
using namespace DirectX;

namespace ascendPrivate
{
	constexpr uint8_t MAX_FRAMES = 2;
}

class ascend
{
public:
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};
	ascend();
	void Initialize();
	void WaitForGPU();
	void OnRender();
	void OnDestroy();
	void MoveToNextFrame();
	void PopulateCommandList();
	

	ComPtr<ID3D12Device4> m_device;
	ComPtr<IDXGIAdapter4> m_adapter;

	ComPtr<IDXGISwapChain4> m_swapChain;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12Resource2> m_renderTargets[ascendPrivate::MAX_FRAMES];
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12RootSignature> m_rootSignature;
	ComPtr<ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3D12CommandAllocator> m_commandAllocators[ascendPrivate::MAX_FRAMES];
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<ID3D12Fence> m_fence;
	ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	CD3DX12_VIEWPORT m_viewport;
	CD3DX12_RECT m_scissorRect;

	UINT m_rtvDescriptorSize;
	UINT m_frameIndex;
	UINT64 m_fenceValues[ascendPrivate::MAX_FRAMES];
	HANDLE m_fenceEvent;

	HWND hwnd = nullptr;

};