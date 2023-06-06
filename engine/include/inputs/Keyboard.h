#pragma once
#include <queue>

enum class KeyState
{
    KEY_UP,
    KEY_DOWN,
    KEY_PRESSED,
    KEY_RELEASED,
    KEY_INVALID
};

class Keyboard
{
// if is Windows 
#ifdef _WIN32
    friend class Window;
#endif
public:
    class KeyEvent
    {
    public:
        KeyEvent(): m_state(KeyState::KEY_INVALID), m_code(0) {}
        KeyEvent(KeyState state, unsigned int code): m_state(state), m_code(code) {}
        ~KeyEvent();
        bool IsValid() const { return m_state != KeyState::KEY_INVALID; }
        KeyState GetState() const { return m_state; }
        unsigned int GetCode() const { return m_code; }
    private :
        KeyState m_state;
        unsigned int m_code;
    };

    Keyboard() = default;
    Keyboard(const Keyboard&) = delete;
    Keyboard& operator=(const Keyboard&) = delete;
    ~Keyboard() {}

    //key events 
    bool KeyPressed(unsigned int) const ;
    bool KeyReleased(unsigned int) const ;
    KeyEvent ReadKey();
    bool KeyEmpty() const { return m_keyBuffer.empty(); }
    void FlushKey();
    //

};