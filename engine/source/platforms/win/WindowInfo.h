#pragma once
#include "EngineWin.h"

struct WindowInfo
{
    int width;
    int height;
    HWND hWnd;
};

struct WinMsg
{
    HWND hwnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
};
