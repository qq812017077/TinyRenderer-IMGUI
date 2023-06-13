#include "Graphics.h"
#include <iostream>

#define LOG(X) std::cout << X << std::endl;
Graphics::Graphics()
{
    LOG("base Graphics  constructor begin")
    // CreateDevice();
    // CreateRenderTarget();
    LOG("base Graphics  constructor end")
}

Graphics::~Graphics()
{
    // ClearupDevice();
}
