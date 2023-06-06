#include "EngineWin.h"
#include <unordered_map>
#include "WindowException.h"

class Window
{
public:
    Window(int width, int height, const wchar_t* name);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    
    static void CloseWindow(HWND & hwnd);
    static int GetWindowCount() noexcept
    {
        return window_map.size();
    }

private:
    LRESULT HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    void ClostWindow() noexcept { ::DestroyWindow(hWnd); }
    int width;
    int height;
    HWND hWnd;

private:
    static std::unordered_map<HWND, Window*> window_map;
    static LRESULT WINAPI HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    static LRESULT WINAPI HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    
    // singloton
    class WindowClass
    {
    public:
        static const wchar_t* GetName() noexcept;
        static HINSTANCE GetInstance() noexcept;
    private:
        WindowClass() noexcept;
        ~WindowClass();
        WindowClass(const WindowClass&) = delete;
        WindowClass& operator=(const WindowClass&) = delete;
        HINSTANCE hInst;

        static constexpr const wchar_t* wndClassName = L"Engine Window";
        static WindowClass wndClass;
    };
};

// error exception macro
#define WND_EXCEPT(hr) WindowException(__LINE__, __FILE__, hr)
#define WND_LAST_EXCEPT() WindowException(__LINE__, __FILE__, GetLastError())