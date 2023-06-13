#include "Exceptions.h"
#include <sstream>
#include "DXErr.h"
/******************************************
 * WindowException
*/
WindowException::WindowException(int line, const char* file, HRESULT hr) noexcept
    :
    hr(hr),
    EngineException(line, file, TranslateErrorCode(hr).c_str())
{}

const char* WindowException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "[Error Code] " << GetErrorCode() << std::endl
        << "[Description] " << GetErrorString() << std::endl
        << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* WindowException::GetType() const noexcept
{
    return "Window Exception";
}

std::string WindowException::TranslateErrorCode(HRESULT hr) noexcept
{
    char* pMsgBuf = nullptr;
    DWORD nMsgLen = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        hr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&pMsgBuf),
        0,
        nullptr
        );
    if (nMsgLen == 0)
    {
        return "Unidentified error code";
    }
    std::string errorString = pMsgBuf;
    LocalFree(pMsgBuf);
    return errorString;
}

HRESULT WindowException::GetErrorCode() const noexcept
{
    return hr;
}

std::string WindowException::GetErrorString() const noexcept
{
    return TranslateErrorCode(hr);
}

/******************************************
 * GraphicsException
*/
GraphicsException::GraphicsException(int line, const char* file, HRESULT hr) noexcept : 
    EngineException(line, file, "Graphics Exception"), 
    hr(hr)
{
}
#ifndef NDEBUG
GraphicsException::GraphicsException(int line, const char* file, HRESULT hr , std::vector<std::string> infoMsgs) noexcept : 
    EngineException(line, file, "Graphics Exception"), 
    hr(hr)
{
    for (const auto& m : infoMsgs)
    {
        info += m;
        info.push_back('\n');
    }
    if (!info.empty())
    {
        info.pop_back();
    }
}
#endif

const char* GraphicsException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
        << std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
        << "[Error String] " << GetErrorString() << std::endl
        << "[Description] " << GetErrorDescription() << std::endl;
    if(!info.empty())
    {
        oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
    }
    oss << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* GraphicsException::GetType() const noexcept
{
    return "Graphics Exception";
}

std::string GraphicsException::TranslateErrorCode(HRESULT hr) noexcept
{
    return WindowException::TranslateErrorCode(hr);
}

HRESULT GraphicsException::GetErrorCode() const noexcept
{
    return hr;
}

std::string GraphicsException::GetErrorString() const noexcept
{
    // convert HRESULT to error code string
    return DXGetErrorString(hr);
}

std::string GraphicsException::GetErrorDescription() const noexcept
{
    char buf[512];
    DXGetErrorDescription(hr, buf, sizeof(buf));
    return buf;
}

std::string GraphicsException::GetErrorInfo() const noexcept
{
    return info;
}

const char* DeviceRemovedException::GetType() const noexcept
{
    return "Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

/******************************************
 * InfoException
*/
InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept : 
    EngineException(line, file, "Graphics Info Exception")
{
    for (const auto& m : infoMsgs)
    {
        info += m;
        info.push_back('\n');
    }
    if (!info.empty())
    {
        info.pop_back();
    }
}

const char* InfoException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
    oss << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* InfoException::GetType() const noexcept
{
    return "Graphics Info Exception";
}

std::string InfoException::GetErrorInfo() const noexcept
{
    return info;
}
