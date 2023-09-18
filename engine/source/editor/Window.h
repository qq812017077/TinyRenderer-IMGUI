#pragma once
#include <iostream>
#include <unordered_map>
#include <optional>
#include <functional>
#include <vector>
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
    int GetWidth() const noexcept { return width; }
    int GetHeight() const noexcept { return height; }
    //Keyboard events
    Keyboard::KeyEvent ReadKeyEvent() { return kbd.ReadKey(); }
    bool GetKey(Input::KeyCode keycode) const noexcept { return kbd.GetKey(keycode); }
    bool GetKeyDown(Input::KeyCode keycode) const noexcept { return kbd.GetKeyDown(keycode); }
    bool GetKeyUp(Input::KeyCode keycode) const noexcept { return kbd.GetKeyUp(keycode); }

    //Mouse events
    Mouse::MouseEvent ReadMouseEvent() { return mouse.Read();}
    Mouse& GetMouse() { return mouse;}
    bool GetMouseButton(Mouse::MouseButton button) const noexcept { return mouse.GetMouseButton(button); }
    bool GetMouseButtonDown(Mouse::MouseButton button) const noexcept { return mouse.GetMouseButtonDown(button); }
    bool GetMouseButtonUp(Mouse::MouseButton button) const noexcept { return mouse.GetMouseButtonUp(button); }
    std::pair<int, int> GetMousePos() const noexcept { return mouse.GetPos(); }

    //Graphics
    std::unique_ptr<Graphics>& Gfx() { return pGfx; }
    //Window 
    virtual void ShowMessageBox(const std::string& title, const std::string& message) const noexcept = 0;
    virtual void SetTitle(const std::wstring& title) {}
    virtual std::optional<int> ProcessMessages() noexcept;
    // static std::optional<int> ProcessMessages() noexcept;

public: // events
    
    typedef std::function<void()>                   onResetFunc;
    typedef std::function<void(int, int)>           onKeyFunc;
    
    typedef std::function<void(unsigned char)>      onCharFunc;
    typedef std::function<void(int, unsigned int)>  onCharModsFunc;
    
    typedef std::function<void(int, int)>           onMouseButtonFunc;
    typedef std::function<void(double, double)>     onCursorPosFunc;

    typedef std::function<void(int)>                onCursorEnterFunc;
    typedef std::function<void(double, double)>     onScrollFunc;
    typedef std::function<void(int, const char**)>  onDropFunc;
    typedef std::function<void(int, int)>           onWindowSizeFunc;
    typedef std::function<void()>                   onWindowCloseFunc;
    
    void        registerOnResetFunc(onResetFunc func) { m_onResetFunc.push_back(func); }
    void        registerOnKeyFunc(onKeyFunc func) { m_onKeyFunc.push_back(func); }
    void        registerOnCharFunc(onCharFunc func) { m_onCharFunc.push_back(func); }
    void        registerOnCharModsFunc(onCharModsFunc func) { m_onCharModsFunc.push_back(func); }
    void        registerOnMouseButtonFunc(onMouseButtonFunc func) { m_onMouseButtonFunc.push_back(func); }
    void        registerOnCursorPosFunc(onCursorPosFunc func) { m_onCursorPosFunc.push_back(func); }
    void        registerOnCursorEnterFunc(onCursorEnterFunc func) { m_onCursorEnterFunc.push_back(func); }
    void        registerOnScrollFunc(onScrollFunc func) { m_onScrollFunc.push_back(func); }
    void        registerOnDropFunc(onDropFunc func) { m_onDropFunc.push_back(func); }
    void        registerOnWindowSizeFunc(onWindowSizeFunc func) { m_onWindowSizeFunc.push_back(func); }
    void        registerOnWindowCloseFunc(onWindowCloseFunc func) { m_onWindowCloseFunc.push_back(func); }

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
    
    std::vector<onResetFunc>       m_onResetFunc;
    std::vector<onKeyFunc>         m_onKeyFunc;
    std::vector<onCharFunc>        m_onCharFunc;
    std::vector<onCharModsFunc>    m_onCharModsFunc;
    std::vector<onMouseButtonFunc> m_onMouseButtonFunc;
    std::vector<onCursorPosFunc>   m_onCursorPosFunc;
    std::vector<onCursorEnterFunc> m_onCursorEnterFunc;
    std::vector<onScrollFunc>      m_onScrollFunc;
    std::vector<onDropFunc>        m_onDropFunc;
    std::vector<onWindowSizeFunc>  m_onWindowSizeFunc;
    std::vector<onWindowCloseFunc> m_onWindowCloseFunc;


    virtual void CloseWindow() noexcept {};
    int width;
    int height;
    Keyboard kbd;
    Mouse mouse;
    std::unique_ptr<Graphics> pGfx;
};