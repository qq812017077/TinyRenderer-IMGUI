#pragma once

class Graphics;
namespace TinyEngine
{
    class BufferResource
    {
        public:
            BufferResource(Graphics* pGfx, unsigned int width, unsigned int height): m_width(width), m_height(height) {}
            ~BufferResource() = default;

            unsigned int GetWidth() {return m_width;}
            unsigned int GetHeight() {return m_height;}
            virtual void BindAsTexture(Graphics* pGfx, unsigned int slot) = 0;
            virtual void Clear(Graphics* pGfx) = 0;
        protected:
            unsigned int m_width;
            unsigned int m_height;
    };
}