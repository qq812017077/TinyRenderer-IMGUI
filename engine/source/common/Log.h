#pragma once

#ifdef _DEBUG
#include <iostream>
#endif

void inline Log(const char* message)
{
#ifdef _DEBUG
    std::cout << message << std::endl;
#endif
}