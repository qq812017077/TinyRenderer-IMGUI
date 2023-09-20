#include "ImguiManager.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx11.h"
#include "DirectXGraphics.h"

ImguiManager::ImguiManager()
{
    ImGui::CreateContext();
}

ImguiManager::~ImguiManager()
{
    ImGui_ImplWin32_Shutdown();
    ImGui_ImplDX11_Shutdown();
    ImGui::DestroyContext();
}

void ImguiManager::Init(Win32Window* pWindow)
{
    hWnd = pWindow->GetHwnd();
    ImGui_ImplWin32_Init(hWnd);
    DirectXGraphics* pGfx = dynamic_cast<DirectXGraphics*>(pWindow->Gfx().get());
    ImGui_ImplDX11_Init(pGfx->GetDevice(), pGfx->GetContext());
    isEnabled = true;
}


void ImguiManager::NewFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImguiManager::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImguiManager::SetEnable(bool enable)
{
    isEnabled = enable;
}
bool ImguiManager::IsEnabled()
{
    return isEnabled;
}