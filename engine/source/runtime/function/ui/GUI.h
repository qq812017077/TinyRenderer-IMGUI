#pragma once
#include "imgui/imgui.h"

namespace GUI = ImGui;

// namespace GUI
// {
//     void Begin(const char * name);
//     void End();
//     void BeginChild(const char * str_id);
//     void EndChild();
//     void Text(const char * fmt, ...);
//     bool Button(const char * label);
//     bool Checkbox(const char * label, bool * v);
//     bool RadioButton(const char * label, bool active);
//     bool InputText(const char * label, char * buf, size_t buf_size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void * user_data = NULL);
//     bool InputFloat(const char * label, float * v, float step = 0.0f, float step_fast = 0.0f, const char * format = "%.3f", ImGuiInputTextFlags flags = 0);
//     bool InputFloat2(const char * label, float v[2], const char * format = "%.3f", ImGuiInputTextFlags flags = 0);
//     bool InputFloat3(const char * label, float v[3], const char * format = "%.3f", ImGuiInputTextFlags flags = 0);
//     bool InputFloat4(const char * label, float v[4], const char * format = "%.3f", ImGuiInputTextFlags flags = 0);
//     bool InputInt(const char * label, int * v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
//     bool InputInt2(const char * label, int v[2], ImGuiInputTextFlags flags = 0);
//     bool InputInt3(const char * label, int v[3], ImGuiInputTextFlags flags = 0);
//     bool InputInt4(const char * label, int v[4], ImGuiInputTextFlags flags = 0);
    

//     using SliderFloat = ImGui::SliderFloat;
//     using SliderFloat2 = ImGui::SliderFloat2;
//     using SliderFloat3 = ImGui::SliderFloat3;
//     using SliderFloat4 = ImGui::SliderFloat4;
//     using SliderInt = ImGui::SliderInt;
//     using SliderInt2 = ImGui::SliderInt2;
//     using SliderInt3 = ImGui::SliderInt3;
//     using SliderInt4 = ImGui::SliderInt4;
//     using ColorEdit3 = ImGui::ColorEdit3;
//     using ColorEdit4 = ImGui::ColorEdit4;
//     using Image = ImGui::Image;
//     using ImageButton = ImGui::ImageButton;
//     using CollapsingHeader = ImGui::CollapsingHeader;
//     using TreeNode = ImGui::TreeNode;
//     using TreePop = ImGui::TreePop;
//     using ListBoxHeader = ImGui::ListBoxHeader;
//     using ListBoxFooter = ImGui::ListBoxFooter;
//     using Selectable = ImGui::Selectable;
//     using BeginMainMenuBar = ImGui::BeginMainMenuBar;
//     using EndMainMenuBar = ImGui::EndMainMenuBar;
//     using BeginMenuBar = ImGui::BeginMenuBar;
//     using EndMenuBar = ImGui::EndMenuBar;
//     using BeginMenu = ImGui::BeginMenu;
//     using EndMenu = ImGui::EndMenu;
//     using MenuItem = ImGui::MenuItem;
//     using BeginTooltip = ImGui::BeginTooltip;
//     using EndTooltip = ImGui::EndTooltip;
//     using OpenPopup = ImGui::OpenPopup;
//     using BeginPopup = ImGui::BeginPopup;
//     using BeginPopupModal = ImGui::BeginPopupModal;
//     using BeginPopupContextItem = ImGui::BeginPopupContextItem;
//     using BeginPopupContextWindow = ImGui::BeginPopupContextWindow;
//     using BeginPopupContextVoid = ImGui::BeginPopupContextVoid;
//     using EndPopup = ImGui::EndPopup;
//     using BeginPopupContextVoid = ImGui::BeginPopupContextVoid;
//     using BeginPopupContextItem = ImGui::BeginPopupContextItem;
//     using BeginPopupContextWindow = ImGui::BeginPopupContextWindow;
//     using EndPopupContextItem = ImGui::EndPopupContextItem;
//     using EndPopupContextWindow = ImGui::EndPopupContextWindow;
//     using EndPopupContextVoid = ImGui::EndPopupContextVoid;
//     using BeginTabBar = ImGui::BeginTabBar;
//     using EndTabBar = ImGui::EndTabBar;
//     using BeginTabItem = ImGui::BeginTabItem;
//     using EndTabItem = ImGui::EndTabItem;
//     using BeginTable = ImGui::BeginTable;
//     using EndTable = ImGui::EndTable;
//     using TableNextRow = ImGui::TableNextRow;
//     using TableNextColumn = ImGui::TableNextColumn;
//     using TableSetColumnIndex = ImGui::TableSetColumnIndex;
//     using TableSetupColumn = ImGui::TableSetupColumn;
//     using TableSetupScrollFreeze = ImGui::TableSetupScrollFreeze;
//     using TableHeadersRow = ImGui::TableHeadersRow;
//     using TableHeader = ImGui::TableHeader;
//     using TableGetColumnCount = ImGui::TableGetColumnCount;
//     using TableGetColumnFlags = ImGui::TableGetColumnFlags;
// }