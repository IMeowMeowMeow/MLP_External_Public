#pragma once
#include "../External/imgui/imgui_impl_win32.h"
#include "../External/imgui/imgui_impl_dx11.h"
#include "../External/imgui/imgui.h" 

namespace Menu {
    inline ImGuiWindowFlags WindowFlags = {
        ImGuiWindowFlags_NoResize
      | ImGuiWindowFlags_NoCollapse
      | ImGuiWindowFlags_NoTitleBar
      | ImGuiWindowFlags_NoDecoration
    };

    inline bool showMenu = false;

    inline float WindowWidth = 900;
    inline float WindowHeight = 700;
    inline float SidebarWidth = 200;
    inline float SidebarHeight = 60;
    inline float SubTabsHeight = 60;

    inline float Rounding = 10.0f;
    inline float LineThickness = 2.0f;

    void InitImGui();
    void Draw();

    void RenderMiscTab();
    void RenderAimbotTab();
    void RenderInventoryTab();
    void RenderPlayerTab();
    void RenderTeleportTab();

    void RenderSettingsSubTabs();
    void RenderSettingsTab();
    void RenderConfigSubTab();
    void RenderMenuSubTab();
    void RenderHotkeysSubTab();


    void RenderVisualsSubTabs();
    void RenderVisualsTab();
}