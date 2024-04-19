#pragma once

#include <stdexcept>

using Microsoft::WRL::ComPtr;

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