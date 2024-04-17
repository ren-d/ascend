#pragma once
#include <iostream>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dx12/d3dx12.h> // include d3d
#include <wrl.h>
#include "WindowsApplication.h"
using namespace Microsoft::WRL;

class Renderer
{
public:
	Renderer();
	~Renderer();
	HRESULT Initialize();
};

