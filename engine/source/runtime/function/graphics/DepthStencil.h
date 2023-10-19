#pragma once
#include "BufferResource.h"

class Graphics;
namespace TinyEngine
{
    class DepthStencil : public BufferResource
    {
        public:
            using BufferResource::BufferResource;
            ~DepthStencil() = default;
    };
}