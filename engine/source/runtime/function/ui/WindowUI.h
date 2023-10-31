#pragma once


namespace TinyEngine
{
    class UIState;
    class WindowUI
    {

    public:
        WindowUI() {}
        int          initialize();
        virtual bool OnTick(UIState* uistate) = 0;
        virtual void draw_frame() = 0;
        virtual void  clear() = 0;
        void setDefaultStyle();
    };
}