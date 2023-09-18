#pragma once


namespace TinyEngine
{
    class UIState;
    class SurfaceUI
    {

    public:
        SurfaceUI() {}
        int          initialize();
        virtual bool OnTick(UIState* uistate) = 0;
        virtual void draw_frame() = 0;
        virtual void  clear() = 0;
        void setDefaultStyle();
    protected:
        virtual void registerInput() = 0;
    };
}