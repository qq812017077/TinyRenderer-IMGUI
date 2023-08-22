#pragma once
#include "EngineWin.h"
#include "Window.h"

class Win32Window : public Window
{
    friend class WindowClass;
    friend class DirectXGraphics;
public:
    Win32Window(int width, int height, const wchar_t* name);
    ~Win32Window();
    Win32Window(const Win32Window&) = delete;
    Win32Window& operator=(const Win32Window&) = delete;
    
    void BindImgui() override;

    //Window 
    void ShowMessageBox(const std::string& title, const std::string& message) const noexcept override;
    void SetTitle(const std::wstring& title);
    std::optional<int> ProcessMessages() noexcept override;
    HWND GetHwnd() const noexcept { return hWnd; }
protected:
    void CloseWindow() noexcept;
private:
    LRESULT HandleMsg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept;
    HWND hWnd;
    bool bindedImgui = false;
};

class WindowClass
{
public:
    static const wchar_t* GetName() noexcept;
    static HINSTANCE GetInstance() noexcept;

    static LRESULT WINAPI HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    static LRESULT WINAPI HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
private:

    WindowClass() noexcept;
    ~WindowClass();
    WindowClass(const WindowClass&) = delete;
    WindowClass& operator=(const WindowClass&) = delete;
    HINSTANCE hInst;

    static LRESULT WINAPI HandleWindowMsg(Window* &wnd, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    static constexpr const wchar_t* wndClassName = L"Engine Window";
    static WindowClass wndClass;
};