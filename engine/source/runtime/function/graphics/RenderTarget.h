#pragma once
#include "BufferResource.h"
class Graphics;

namespace TinyEngine
{
    class RenderTarget : public BufferResource
    {
        public:
            using BufferResource::BufferResource;
            ~RenderTarget() = default;
    };
}