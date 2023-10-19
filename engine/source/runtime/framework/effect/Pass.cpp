#include "Pass.h"
#include "graphics/Graphics.h"

namespace TinyEngine
{
    void RenderState::Apply(Graphics* pGfx) { 
        pGfx->ApplyState(this); 
    }
}