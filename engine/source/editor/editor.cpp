#include "Editor.h"
#include "Engine.h"
#include <cassert>
#include <iomanip>
#include "core/EngineException.h"
#ifdef _WIN32
#include "Win32Window.h"
#elif __linux__
#endif

namespace TinyEngine
{
    Editor::Editor()
    {
    }

    Editor::~Editor()
    {
    }

    void Editor::Initialize(Engine* engine_runtime)
    {
        assert(engine_runtime);
        m_engine_runtime = engine_runtime;
        m_editor_ui      = std::make_shared<EditorUI>(this);
        m_editor_ui->initialize();
        // if win32 , use win32 window
#ifdef _WIN32
        pWnd = std::make_unique<Win32Window>(1280, 720, L"TinyRenderer");
        imgui.Init(dynamic_cast<Win32Window *>(pWnd.get()));
#elif __linux__
        pWnd = std::make_unique<LinuxWindow>(1280, 720, L"TinyRenderer");
#endif
        engine_runtime->BindEditor(m_editor_ui.get());
        engine_runtime->m_graphics = pWnd->Gfx().get();
    }

    int Editor::Run()
    {
        assert(m_engine_runtime);
        try
        {
            m_engine_runtime->Run();
            return 0;
        }
        catch(const EngineException& e)
        {
            pWnd->ShowMessageBox(e.what(), e.GetType());
            // MessageBox(pWnd->GetWindowHandle(), e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
            // focus on the message box
            // SetForegroundWindow(pWnd->GetWindowHandle());
        }
        catch(const std::exception& e)
        {
            pWnd->ShowMessageBox(e.what(), "Standard Exception");
            // MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
        }
        catch(...)
        {
            pWnd->ShowMessageBox("No details available", "Unknown Exception");
            // MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
        }
        return -1;
    }

    
    void Editor::Clear()
    {
        m_engine_runtime = nullptr;
    }

    
    void   Editor::onWindowChanged(float pos_x, float pos_y, float width, float height) const
    {
        // we need to reset camera aspect
        // we need to reset viewport
        pWnd->Gfx()->UpdateRenderSceneViewPort(static_cast<int>(pos_x), static_cast<int>(pos_y), static_cast<int>(width), static_cast<int>(height));
    }
};


