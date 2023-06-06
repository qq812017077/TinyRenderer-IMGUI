#pragma once
#include <queue>

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

class Mouse
{
#ifdef _WIN32
    friend class Window;
#endif

public:
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
    //mouse events
    bool LeftPressed() const { return m_leftIsPressed; }
    bool LeftReleased() const { return !m_leftIsPressed; }
    bool RightPressed() const { return m_rightIsPressed; }
    bool RightReleased() const { return !m_rightIsPressed; }
    bool WheelPressed() const { return m_wheelIsPressed; }
    bool WheelReleased() const { return !m_wheelIsPressed; }
    bool IsInWindow() const { return m_isInWindow; }
    std::pair<int,int> GetPos() const { return { m_x, m_y }; }
    int GetPosX() const { return m_x; }
    int GetPosY() const { return m_y; }
    //read
    MouseEvent Read() { if (m_mouseBuffer.size() > 0) { MouseEvent e = m_mouseBuffer.front(); m_mouseBuffer.pop(); return e; } else { return MouseEvent(); } }
    //flush
    void Flush() { m_mouseBuffer = std::queue<MouseEvent>(); }
private:
    void OnLeftPressed() noexcept { m_leftIsPressed = true; m_mouseBuffer.push(MouseEvent(MouseState::LEFT_PRESSED, *this)); TrimBuffer(); }
    void OnLeftReleased() noexcept { m_leftIsPressed = false; m_mouseBuffer.push(MouseEvent(MouseState::LEFT_RELEASED, *this)); TrimBuffer(); }
    void OnRightPressed() noexcept { m_rightIsPressed = true; m_mouseBuffer.push(MouseEvent(MouseState::RIGHT_PRESSED, *this)); TrimBuffer(); }
    void OnRightReleased() noexcept { m_rightIsPressed = false; m_mouseBuffer.push(MouseEvent(MouseState::RIGHT_RELEASED, *this)); TrimBuffer(); }
    void OnWheelPressed() noexcept { m_wheelIsPressed = true; m_mouseBuffer.push(MouseEvent(MouseState::WHEEL_PRESSED, *this)); TrimBuffer(); }
    void OnWheelReleased() noexcept { m_wheelIsPressed = false; m_mouseBuffer.push(MouseEvent(MouseState::WHEEL_RELEASED, *this)); TrimBuffer(); }
    
    void OnWheelUp() noexcept { m_mouseBuffer.push(MouseEvent(MouseState::WHEEL_UP, *this)); TrimBuffer(); }
    void OnWheelDown() noexcept { m_mouseBuffer.push(MouseEvent(MouseState::WHEEL_DOWN, *this)); TrimBuffer(); }
    void OnWheelDelta(int delta) noexcept {
        m_wheelDeltaCarry += delta;
        while (m_wheelDeltaCarry >= WHEEL_DELTA)
        {
            m_wheelDeltaCarry -= WHEEL_DELTA;
            OnWheelUp();
        }
        while (m_wheelDeltaCarry <= -WHEEL_DELTA)
        {
            m_wheelDeltaCarry += WHEEL_DELTA;
            OnWheelDown();
        }
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
    bool m_leftIsPressed = false;
    bool m_rightIsPressed = false;
    
    bool m_wheelIsPressed = false;
    int m_wheelDeltaCarry = 0;

    bool m_isInWindow = false;
    int m_x = 0;
    int m_y = 0;

    static constexpr unsigned int bufferSize = 16u;
};