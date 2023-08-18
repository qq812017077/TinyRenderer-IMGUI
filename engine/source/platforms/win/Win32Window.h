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
    
    //Keyboard events
    Keyboard::KeyEvent ReadKeyEvent() { return kbd.ReadKey(); }
    bool IsKeyPressed(unsigned char keycode) const noexcept { return kbd.KeyPressed(keycode); }
    bool IsKeyReleased(unsigned char keycode) const noexcept { return kbd.KeyReleased(keycode); }
    
    //Mouse events
    Mouse::MouseEvent ReadMouseEvent() { return mouse.Read();}
    Mouse& GetMouse() { return mouse;}
    bool IsLeftPressed() const noexcept { return mouse.LeftPressed(); }
    bool IsRightPressed() const noexcept { return mouse.RightPressed(); }
    bool IsMiddlePressed() const noexcept { return mouse.WheelPressed(); }
    bool IsLeftReleased() const noexcept { return mouse.LeftReleased(); }
    bool IsRightReleased() const noexcept { return mouse.RightReleased(); }
    bool IsMiddleReleased() const noexcept { return mouse.WheelReleased(); }
    std::pair<int, int> GetMousePos() const noexcept { return mouse.GetPos(); }

    //Graphics
    Graphics& Gfx() { return *pGfx; }
    
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