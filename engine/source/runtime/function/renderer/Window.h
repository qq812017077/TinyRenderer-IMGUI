#pragma once
#include <iostream>
#include <unordered_map>
#include <optional>

#include "Graphics.h"

#include "WindowInfo.h"
#include "Keyboard.h"
#include "Mouse.h"

class Window
{
public:
    Window(int width, int height, const wchar_t* name);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    WindowInfo GetWindowInfo() const noexcept { return windowInfo; }
    
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
    void SetTitle(const std::wstring& title);
    static std::optional<int> ProcessMessages() noexcept;
    // static void CloseWindow(WindowInfo & window);
    
    LRESULT HandleMsg(WinMsg message) noexcept;
private:
    void CloseWindow() noexcept;
    WindowInfo windowInfo;
    Keyboard kbd;
    Mouse mouse;
    std::unique_ptr<Graphics> pGfx;
private:
    
};
