#pragma once
#include "EngineWin.h"
#include "Window.h"

// singloton
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