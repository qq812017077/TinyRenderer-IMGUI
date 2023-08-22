#pragma once
#include <iostream>
#include <unordered_map>
#include <optional>
#include "Graphics.h"
#include "input/Keyboard.h"
#include "input/Mouse.h"
class   Window
{
protected:
    Window(int width, int height, const wchar_t* name);
public:
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    virtual void BindImgui() = 0;
    virtual void Update();
    //Keyboard events
    Keyboard::KeyEvent ReadKeyEvent() { return kbd.ReadKey(); }
    bool GetKey(Input::KeyCode keycode) const noexcept { return kbd.GetKey(keycode); }
    bool GetKeyDown(Input::KeyCode keycode) const noexcept { return kbd.GetKeyDown(keycode); }
    bool GetKeyUp(Input::KeyCode keycode) const noexcept { return kbd.GetKeyUp(keycode); }

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
    virtual void ShowMessageBox(const std::string& title, const std::string& message) const noexcept = 0;
    virtual void SetTitle(const std::wstring& title) {}
    virtual std::optional<int> ProcessMessages() noexcept;
    // static std::optional<int> ProcessMessages() noexcept;

protected:
    void ClearKeyboard() noexcept;
    void OnKeyPressed(unsigned char keycode) noexcept;
    void OnKeyReleased(unsigned char keycode) noexcept;
    void OnChar(unsigned char character) noexcept;
    void OnMouseMove(int x, int y) noexcept;
    void OnMouseLeave() noexcept;
    void OnMouseEnter() noexcept;
    void OnMouseLeftPressed() noexcept;
    void OnMouseLeftReleased() noexcept;
    void OnMouseRightPressed() noexcept;
    void OnMouseRightReleased() noexcept;
    void OnMouseWheelPressed() noexcept;
    void OnMouseWheelReleased() noexcept;
    void OnMouseWheelUp() noexcept;
    void OnMouseWheelDown() noexcept;
    void OnMouseWheelDelta(int delta) noexcept;

    virtual void CloseWindow() noexcept {};
    int width;
    int height;
    Keyboard kbd;
    Mouse mouse;
    std::unique_ptr<Graphics> pGfx;
};