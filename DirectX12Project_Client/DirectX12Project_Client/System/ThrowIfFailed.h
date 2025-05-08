#ifndef THROW_IF_FAILED_H
#define THROW_IF_FAILED_H

#include <Windows.h>
#include <comdef.h>
#include <stdexcept>
#include <string>

inline std::wstring ConvertToWString(const char* str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    std::wstring result(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str, -1, &result[0], size_needed);
    return result;
}

inline void ThrowIfFailed(HRESULT result, const char* file, int line, const char* function)
{
    if (FAILED(result))
    {
        _com_error error(result);
        std::wstring error_msg = ConvertToWString(file) + L" - " + std::to_wstring(line) + L" - " + ConvertToWString(function) + L" : HRESULT failed - " + std::wstring(error.ErrorMessage());

        MessageBoxW(nullptr, error_msg.c_str(), L"DirectX Error", MB_ICONERROR | MB_OK);

        throw error_msg;
    }
}

#define THROW_IF_FAILED(hr) ThrowIfFailed((hr), __FILE__, __LINE__, __FUNCTION__)

#endif
