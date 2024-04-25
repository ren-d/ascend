#include "D3D12Core.h"
#include "ascendHelpers.h"
#include <d3dcompiler.h>
ascend::ascend() : m_viewport(0.0f, 0.0f, static_cast<LONG>(800), static_cast<LONG>(800)), m_scissorRect(0, 0, 800, 800)
{

}
void ascend::Initialize()
{
	
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
		if (SUCCEEDED(D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device))))
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
	swapChainDesc.BufferCount = ascendPrivate::MAX_FRAMES;
	swapChainDesc.Width = 800;
	swapChainDesc.Height = 800;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	// TODO: Define all descs

	ComPtr<IDXGISwapChain1> tempSwapChain;

	// TODO: Add fullscreen support
	VERIFYD3D12RESULT(m_factory->CreateSwapChainForHwnd(m_commandQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, &tempSwapChain));	// use CreateSwapChainForCoreWindow for Windows Store apps

	VERIFYD3D12RESULT(m_factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER)); // disables full screen

	// TODO: follow d3d12 Samples member variables
	VERIFYD3D12RESULT(tempSwapChain.As(&m_swapChain)); // seems like you can only create a swapchain1. Therefore to get currentBackbuffer

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex(); // DirectX's "MiniEngine" does not do this?

	// create descriptor heaps
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = ascendPrivate::MAX_FRAMES;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // create render target descriptor heap.
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	VERIFYD3D12RESULT(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));
	m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

	ComPtr<ID3D12Resource> renderTargets[3];
	for (UINT n = 0; n < ascendPrivate::MAX_FRAMES; ++n)
	{
		VERIFYD3D12RESULT(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
		m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, m_rtvDescriptorSize);

		VERIFYD3D12RESULT(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[n])));
	}



	// Create Empty root singature
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;

	VERIFYD3D12RESULT(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &signature, &error));
	VERIFYD3D12RESULT(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));

	// pipeline state (shaders)
	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;

	UINT compileFlags = 0;
#if DEBUG
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	// compile shaders
	VERIFYD3D12RESULT(D3DCompileFromFile(GetShader(L"shaders.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
	VERIFYD3D12RESULT(D3DCompileFromFile(GetShader(L"shaders.hlsl").c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

	// Define vertex input layout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// Create Pipeline State Object
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = m_rootSignature.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;

	VERIFYD3D12RESULT(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));

	// Create the command list.
	VERIFYD3D12RESULT(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[m_frameIndex].Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_commandList)));

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	VERIFYD3D12RESULT(m_commandList->Close());
	// create vertex buffer
		// Define the geometry for a triangle.
	float aspect = 800 / 800;
	Vertex triangleVertices[] =
	{
		{ { 0.0f, 0.25f * aspect, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		{ { 0.25f, -0.25f * aspect, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { -0.25f, -0.25f * aspect , 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
	};
	const UINT vertexBufferSize = sizeof(triangleVertices);

	// TODO: Refactor to use Default Heap
	const CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_UPLOAD);
	const CD3DX12_RESOURCE_DESC rdesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);

	VERIFYD3D12RESULT(m_device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &rdesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_vertexBuffer)));

	UINT8* pVertexDataBegin;
	CD3DX12_RANGE  readRange(0, 0);
	VERIFYD3D12RESULT(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
	memcpy(pVertexDataBegin, triangleVertices, vertexBufferSize);
	m_vertexBuffer->Unmap(0, nullptr);

	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = sizeof(Vertex);
	m_vertexBufferView.SizeInBytes = vertexBufferSize;

	// syncro objects (fences)

	VERIFYD3D12RESULT(m_device->CreateFence(m_fenceValues[m_frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
	m_fenceValues[m_frameIndex]++;

	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_fenceEvent == nullptr)
	{
		VERIFYD3D12RESULT(HRESULT_FROM_WIN32(GetLastError()));
	}

	WaitForGPU();
}
void ascend::PopulateCommandList()
{
	VERIFYD3D12RESULT(m_commandAllocators[m_frameIndex]->Reset());

	VERIFYD3D12RESULT(m_commandList->Reset(m_commandAllocators[m_frameIndex].Get(), m_pipelineState.Get()));

	m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
	m_commandList->RSSetViewports(1, &m_viewport);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	const CD3DX12_RESOURCE_BARRIER presentTransition = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_commandList->ResourceBarrier(1, &presentTransition);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
	m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	// Record commands.
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	m_commandList->DrawInstanced(3, 1, 0, 0);


	const CD3DX12_RESOURCE_BARRIER backBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// Indicate that the back buffer will now be used to present.
	m_commandList->ResourceBarrier(1, &backBufferTransition);

	VERIFYD3D12RESULT(m_commandList->Close());
}

void ascend::OnRender()
{
	// Record all the commands we need to render the scene into the command list.
	PopulateCommandList();

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Present the frame.
	VERIFYD3D12RESULT(m_swapChain->Present(1, 0));

	MoveToNextFrame();
}

void ascend::OnDestroy()
{
	// Ensure that the GPU is no longer referencing resources that are about to be
// cleaned up by the destructor.
	WaitForGPU();

	CloseHandle(m_fenceEvent);
}

void ascend::WaitForGPU()
{
	// Schedule a Signal command in the queue.
	VERIFYD3D12RESULT(m_commandQueue->Signal(m_fence.Get(), m_fenceValues[m_frameIndex]));

	// Wait until the fence has been processed.
	VERIFYD3D12RESULT(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
	WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

	// Increment the fence value for the current frame.
	m_fenceValues[m_frameIndex]++;
}
void ascend::MoveToNextFrame()
{
	const UINT64 currentFenceValue = m_fenceValues[m_frameIndex];
	// Schedule Signal command
	VERIFYD3D12RESULT(m_commandQueue->Signal(m_fence.Get(), m_fenceValues[m_frameIndex]));

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	// If the next frame is not ready to be rendered yet, wait until it is ready.
	if (m_fence->GetCompletedValue() < m_fenceValues[m_frameIndex])
	{
		VERIFYD3D12RESULT(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
		WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
	}
	m_fenceValues[m_frameIndex] = currentFenceValue + 1;
}