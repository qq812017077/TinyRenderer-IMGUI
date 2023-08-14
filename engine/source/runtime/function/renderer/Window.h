#pragma once
#include <iostream>
#include <unordered_map>
#include <optional>
#include "Graphics.h"
#include "input/Keyboard.h"
#include "input/Mouse.h"

class Window
{
protected:
    Window(int width, int height, const wchar_t* name);
public:
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

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
    std::unique_ptr<Graphics>& Gfx() { return pGfx; }
    //Window 
    virtual void SetTitle(const std::wstring& title) {}
    virtual std::optional<int> ProcessMessages() noexcept;
    // static std::optional<int> ProcessMessages() noexcept;

protected:
    void ClearKeyboard() noexcept { kbd.ClearState(); }
    void OnKeyPressed(unsigned char keycode) noexcept { kbd.OnKeyPressed(keycode); }
    void OnKeyReleased(unsigned char keycode) noexcept { kbd.OnKeyReleased(keycode); }
    void OnChar(unsigned char character) noexcept { kbd.OnChar(character); }
    void OnMouseMove(int x, int y) noexcept { mouse.OnMouseMove(x, y); }
    void OnMouseLeave() noexcept { mouse.OnMouseLeave(); }
    void OnMouseEnter() noexcept { mouse.OnMouseEnter(); }
    void OnMouseLeftPressed() noexcept { mouse.OnLeftPressed(); }
    void OnMouseLeftReleased() noexcept { mouse.OnLeftReleased(); }
    void OnMouseRightPressed() noexcept { mouse.OnRightPressed(); }
    void OnMouseRightReleased() noexcept { mouse.OnRightReleased(); }
    void OnMouseWheelPressed() noexcept { mouse.OnWheelPressed(); }
    void OnMouseWheelReleased() noexcept { mouse.OnWheelReleased(); }
    void OnMouseWheelUp() noexcept { mouse.OnWheelUp(); }
    void OnMouseWheelDown() noexcept { mouse.OnWheelDown(); }
    void OnMouseWheelDelta(int delta) noexcept { mouse.OnWheelDelta(delta); }

    virtual void CloseWindow() noexcept {};
    int width;
    int height;
    Keyboard kbd;
    Mouse mouse;
    std::unique_ptr<Graphics> pGfx;
};