#pragma once
#include "EngineException.h"
#include "EngineWin.h"
#include <string>
#include <vector>
class GraphicsException : public EngineException
    {
        public:
            GraphicsException(int line, const char* file, HRESULT hr) noexcept;
#ifndef NDEBUG
            GraphicsException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept;
#endif
            const char* what() const noexcept override;
            const char* GetType() const noexcept override;
            
            static std::string TranslateErrorCode(HRESULT hr) noexcept;

            HRESULT GetErrorCode() const noexcept;
            std::string GetErrorString() const noexcept;
            std::string GetErrorDescription() const noexcept;
#ifndef NDEBUG
            std::string GetErrorInfo() const noexcept;
#endif
        private:
            HRESULT hr;
            std::string info;
    };
    class DeviceRemovedException : public GraphicsException
    {
        using GraphicsException::GraphicsException;
        public:
            const char* GetType() const noexcept override;
    };

    class WindowException : public EngineException
{
public:
    WindowException(int line, const char* file, HRESULT hr) noexcept;
    const char* what() const noexcept override;
    virtual const char* GetType() const noexcept override;
    static std::string TranslateErrorCode(HRESULT hr) noexcept;
    HRESULT GetErrorCode() const noexcept;
    std::string GetErrorString() const noexcept;
private:
    HRESULT hr;
};


#define GFX_EXCEPT_NOINFO(hr) GraphicsException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw GraphicsException(__LINE__,__FILE__,hr)

#ifndef NDEBUG
#define GFX_EXCEPT(hr) GraphicsException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#else
#define GFX_EXCEPT(hr) GraphicsException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) DeviceRemovedException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO_ONLY(call) (call)
#endif


#define WND_EXCEPT(hr) WindowException(__LINE__, __FILE__, hr)
#define WND_LAST_EXCEPT() WindowException(__LINE__, __FILE__, GetLastError())