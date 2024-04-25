#pragma once
#include <wrl.h>

#include <stdexcept>

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

inline void GetAssetPath(_Out_writes_(pathSize) WCHAR* path, UINT pathSize)
{
    if (path == nullptr)
    {
        throw std::exception();
    }

    DWORD size = GetModuleFileName(nullptr, path, pathSize);
    if (size == 0 || size == pathSize)
    {
        // Method failed or path was truncated.
        throw std::exception();
    }

    WCHAR* lastSlash = wcsrchr(path, L'\\');
    if (lastSlash)
    {
        *(lastSlash + 1) = L'\0';
    }
}

// refactor function to calculate asset path on init
std::wstring GetShader(LPCWSTR shaderFile)
{
    std::wstring shaderPath = L"/Shader/";
    WCHAR path[512];
    shaderPath += shaderFile;
    GetAssetPath(path, _countof(path));
    std::wstring assetPaths = path;
    shaderPath = assetPaths + shaderPath;

    return shaderPath;
}
