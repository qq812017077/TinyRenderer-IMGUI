#pragma once
#include <unordered_map>
#include "EngineWin.h"
#include <string>

class WindowMsgMap
{
public:
    WindowMsgMap() noexcept;
    std::string operator()(UINT msg, LPARAM lParam, WPARAM wParam) const noexcept;
private:
    std::unordered_map<UINT, std::string> map;
};
