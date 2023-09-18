#pragma once
#include <queue>
#include "platform.h"

enum class MouseState
{
    LEFT_PRESSED,
    LEFT_RELEASED,
    RIGHT_PRESSED,
    RIGHT_RELEASED,
    WHEEL_PRESSED,
    WHEEL_RELEASED,
    WHEEL_UP,
    WHEEL_DOWN,
    MOVE,
    ENTER,
    LEAVE,
    INVALID
};

namespace Input
{
    enum MouseButton: int
    {
        Left = 0,
        Right = 1,
        Middle = 2
    };

    enum MouseButtonState : int
    {
        PRESSED = 0,
        RELEASED = 1
    };
}

class Mouse
{
friend class Window;

public:
    enum MouseButton : unsigned char
    {
        Left = 0,
        Right = 1,
        Middle = 2
    };
    class MouseEvent
    {
    public:
        MouseEvent() : m_state(MouseState::INVALID), m_x(0), m_y(0) {}
        MouseEvent(MouseState state, Mouse & parent) : m_state(state),m_x(parent.m_x), m_y(parent.m_y) {}
        ~MouseEvent() {}
        
        MouseState GetState() const { return m_state; }
        bool IsValid() const { return m_state != MouseState::INVALID; }
        bool IsLeftPressed() const { return m_state == MouseState::LEFT_PRESSED; }
        bool IsLeftReleased() const { return m_state == MouseState::LEFT_RELEASED; }
        bool IsRightPressed() const { return m_state == MouseState::RIGHT_PRESSED; }
        bool IsRightReleased() const { return m_state == MouseState::RIGHT_RELEASED; }
        bool IsWheelPressed() const { return m_state == MouseState::WHEEL_PRESSED; }
        bool IsWheelReleased() const { return m_state == MouseState::WHEEL_RELEASED; }
        bool IsWheelUp() const { return m_state == MouseState::WHEEL_UP; }
        bool IsWheelDown() const { return m_state == MouseState::WHEEL_DOWN; }
        bool IsMoved() const { return m_state == MouseState::MOVE; }
        std::pair<int,int> GetPos() const { return { m_x, m_y }; }
        int GetPosX() const { return m_x; }
        int GetPosY() const { return m_y; }
    private :
        MouseState m_state;
        int m_x;
        int m_y;
    };

    Mouse() = default;
    Mouse(const Mouse&) = delete;
    Mouse& operator=(const Mouse&) = delete;
    ~Mouse() {}

    bool IsEmpty() const { return m_mouseBuffer.empty(); }
    void Tick() noexcept { m_MouseStatePrev = m_MouseState; m_last_x = m_x; m_last_y = m_y; m_wheelDeltaCarry = 0; }
    //mouse events
    bool GetMouseButton(MouseButton button) const { return m_MouseState[button]; }
    bool GetMouseButtonDown(MouseButton button) const { return !m_MouseStatePrev[button] && m_MouseState[button]; }
    bool GetMouseButtonUp(MouseButton button) const { return m_MouseStatePrev[button] && !m_MouseState[button]; }

    bool IsInWindow() const { return m_isInWindow; }
    std::pair<int,int> GetPos() const { return { m_x, m_y }; }
    int GetPosX() const { return m_x; }
    int GetPosY() const { return m_y; }
    float GetMouseAxisX() const { return float(m_x - m_last_x) * mouseSensitivity; }
    float GetMouseAxisY() const { return float(m_last_y - m_y) * mouseSensitivity; }
    float GetWhellScroll() const { return m_wheelDeltaCarry * mouseSensitivity; }
    //read
    MouseEvent Read() { if (m_mouseBuffer.size() > 0) { MouseEvent e = m_mouseBuffer.front(); m_mouseBuffer.pop(); return e; } else { return MouseEvent(); } }
    //flush
    void Flush() { m_mouseBuffer = std::queue<MouseEvent>(); }
private:
    void OnLeftPressed() noexcept { m_MouseState[MouseButton::Left] = true; m_mouseBuffer.push(MouseEvent(MouseState::LEFT_PRESSED, *this)); TrimBuffer(); }
    void OnLeftReleased() noexcept { m_MouseState[MouseButton::Left] = false; m_mouseBuffer.push(MouseEvent(MouseState::LEFT_RELEASED, *this)); TrimBuffer(); }
    void OnRightPressed() noexcept { m_MouseState[MouseButton::Right] = true; m_mouseBuffer.push(MouseEvent(MouseState::RIGHT_PRESSED, *this)); TrimBuffer(); }
    void OnRightReleased() noexcept { m_MouseState[MouseButton::Right] = false; m_mouseBuffer.push(MouseEvent(MouseState::RIGHT_RELEASED, *this)); TrimBuffer(); }
    void OnWheelPressed() noexcept { m_MouseState[MouseButton::Middle] = true; m_mouseBuffer.push(MouseEvent(MouseState::WHEEL_PRESSED, *this)); TrimBuffer(); }
    void OnWheelReleased() noexcept { m_MouseState[MouseButton::Middle] = false; m_mouseBuffer.push(MouseEvent(MouseState::WHEEL_RELEASED, *this)); TrimBuffer(); }
    
    void OnWheelUp() noexcept {m_mouseBuffer.push(MouseEvent(MouseState::WHEEL_UP, *this)); TrimBuffer(); }
    void OnWheelDown() noexcept {m_mouseBuffer.push(MouseEvent(MouseState::WHEEL_DOWN, *this)); TrimBuffer(); }
    void OnWheelDelta(int delta) noexcept {
        // m_wheelStateDelta += delta;
        m_wheelDeltaCarry += delta;
        if (m_wheelDeltaCarry > 0) OnWheelUp();
        else if (m_wheelDeltaCarry < 0) OnWheelDown();
        // while (m_wheelDeltaCarry >= WHEEL_DELTA)
        // {
        //     m_wheelDeltaCarry -= WHEEL_DELTA;
        //     OnWheelUp();
        // }
        // while (m_wheelDeltaCarry <= -WHEEL_DELTA)
        // {
        //     m_wheelDeltaCarry += WHEEL_DELTA;
        //     OnWheelDown();
        // }
    }

    void OnMouseMove(int x, int y) noexcept { m_x = x; m_y = y; m_mouseBuffer.push(MouseEvent(MouseState::MOVE, *this)); TrimBuffer(); }
    void OnMouseEnter() noexcept {m_isInWindow = true; m_mouseBuffer.push(MouseEvent(MouseState::ENTER, *this)); TrimBuffer(); }
    void OnMouseLeave() noexcept {m_isInWindow = false; m_mouseBuffer.push(MouseEvent(MouseState::LEAVE, *this)); TrimBuffer(); }
    
    void TrimBuffer() noexcept
    {
        while (m_mouseBuffer.size() > bufferSize)
        {
            m_mouseBuffer.pop();
        }
    }


    std::queue<MouseEvent> m_mouseBuffer;
    std::bitset<3> m_MouseState;
    std::bitset<3> m_MouseStatePrev;

    int m_wheelDeltaCarry = 0;
    bool m_isInWindow = false;
    int m_x = 0;
    int m_y = 0;
    int m_last_x = 0;
    int m_last_y = 0;
    float mouseSensitivity = 0.1f;

    static constexpr unsigned int bufferSize = 16u;
};