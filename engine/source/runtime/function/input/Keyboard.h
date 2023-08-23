#pragma once
#include <queue>
#include <bitset>
#include "KeyCode.h"
enum class KeyState
{
    KEY_PRESSED,
    KEY_RELEASED,
    KEY_INVALID
};

class Keyboard
{
// if is Windows 
friend class Window;

public:
    class KeyEvent
    {
    public:
        KeyEvent(): m_state(KeyState::KEY_INVALID), m_code(0) {}
        KeyEvent(KeyState state, unsigned char code): m_state(state), m_code(code) {}
        ~KeyEvent() {}
        bool IsValid() const { return m_state != KeyState::KEY_INVALID; }
        bool IsPressed() const { return m_state == KeyState::KEY_PRESSED; }
        bool IsReleased() const { return m_state == KeyState::KEY_RELEASED; }
        KeyState GetState() const { return m_state; }
        unsigned char GetCode() const { return m_code; }
    private :
        KeyState m_state;
        unsigned char m_code;
    };

    Keyboard() = default;
    Keyboard(const Keyboard&) = delete;
    Keyboard& operator=(const Keyboard&) = delete;
    ~Keyboard() {}

    void Update() noexcept { m_keyStatesPrev = m_keyStates; }
    //key events 
    bool GetKey(Input::KeyCode keycode) const { return m_keyStates[keycode]; }
    bool GetKeyDown(Input::KeyCode keycode) const
    {
        return !m_keyStatesPrev[keycode] && m_keyStates[keycode]; 
    }
    bool GetKeyUp(Input::KeyCode keycode) const { return m_keyStatesPrev[keycode] && !m_keyStates[keycode]; }
    //read
    KeyEvent ReadKey() { if (m_keyBuffer.size() > 0) { KeyEvent e = m_keyBuffer.front(); m_keyBuffer.pop(); return e; } else { return KeyEvent(); } }
    char ReadChar() { if (m_charBuffer.size() > 0) { unsigned char e = m_charBuffer.front(); m_charBuffer.pop(); return e; } else { return 0; } }
    //empty
    bool KeyEmpty() const { return m_keyBuffer.empty(); }
    bool CharEmpty() const { return m_charBuffer.empty(); }
    //flush
    void FlushKey() { m_keyBuffer = std::queue<KeyEvent>(); }
    void FlushChar() { m_charBuffer = std::queue<unsigned char>(); }
    void Flush() { FlushKey(); FlushChar(); }
    //repeat
    bool AllowRepeat() const { return m_allowRepeat; }
    void EnableRepeat() { m_allowRepeat = true; }
    void DisableRepeat() { m_allowRepeat = false; }
private:
    void OnKeyPressed(unsigned char keycode) noexcept 
    {
        m_keyStates[keycode] = true; 
        m_keyBuffer.push(KeyEvent(KeyState::KEY_PRESSED, keycode));  
        TrimBuffer(m_keyBuffer);
    }
    void OnKeyReleased(unsigned char keycode) noexcept { 
        m_keyStates[keycode] = false; 
        m_keyBuffer.push(KeyEvent(KeyState::KEY_RELEASED, keycode)); 
        TrimBuffer(m_keyBuffer);
    }
    void OnChar(unsigned char character) noexcept { m_charBuffer.push(character); TrimBuffer(m_charBuffer);}
    void ClearState() noexcept{ m_keyStates.reset();}
    
    template<typename T>
    static void TrimBuffer(std::queue<T>& buffer) noexcept
    {
        while (buffer.size() > bufferSize)
        {
            buffer.pop();
        }
    }
    static constexpr unsigned int keyNum = 256u;
    static constexpr unsigned int bufferSize = 16u;

    std::bitset<keyNum> m_keyStates{};
    std::bitset<keyNum> m_keyStatesPrev{};
    std::queue<KeyEvent> m_keyBuffer{};
    std::queue<unsigned char> m_charBuffer{};
    bool m_allowRepeat = false;
};