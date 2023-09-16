#pragma once


namespace TinyEngine
{
    class UIState;
    class SurfaceUI
    {

    public:
        SurfaceUI() {}

        virtual bool OnTick(UIState* uistate) = 0;
        virtual void draw_ui() = 0;
        virtual void  clear() = 0;
    };
}