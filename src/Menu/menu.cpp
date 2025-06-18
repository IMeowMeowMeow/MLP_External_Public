#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui_internal.h>

#include "../MLP/Settings/settings.h"
#include "../External/fonts/fonts.h"
#include "../MLP/Modules/modules.h"
#include "../MLP/Headers/util.hpp"
#include "../MLP/Headers/ItemProperties.h"
#include "../MLP/Headers/globals.h"
#include "../MLP/Headers/enums.h"
#include "../MLP/Headers/game_function.hpp"
#include "../MLP/Headers/game_structures.hpp"
#include "../MLP/Headers/Colors.hpp"
#include "../MLP/Headers/KeyManager.h"
#include "../Menu/Gui/gui_interface.h"
#include "../Menu/Widgets/widgets.h"
#include <iostream>
#include <thread>
#include "menu.h"
#include <mutex>
#include <fonts/Icons/IconsFontAwesome6.h>
#include <io.h>
#include <unordered_set>
#include "../MLP/Headers/structures.hpp"
using namespace util;

using namespace globals;
using namespace Menu;


Tab currentTab = Tab::Aimbot;

static bool previousState = false;

void Menu::Draw() {

    static POINT savedMousePos = { 0, 0 }; 

    if (Settings::ESP::Players::enabled)
        Modules::ESP::Players::Draw();

    if (Settings::ESP::Items::enabled)
        Modules::ESP::Items::Draw();

    if (Settings::Aimbot::drawFOV)
        Modules::Aimbot::DrawFOV(Settings::Aimbot::fov);

    if (Settings::Misc::fovTp && Settings::Misc::drawFov)
        Modules::Aimbot::DrawFOV(Settings::Misc::fovTpRadius);

    if (KeyManager::IsKeyToggled(Settings::Menu::hotkey)) {
        showMenu = !showMenu;
    }


    if (showMenu != previousState) {
        HWND window_handle = overlay->get_window_handle();
        HWND gameWindow = game_window;

        if (!game_window_active) {
            SetWindowLongPtr(window_handle, GWL_EXSTYLE, GetWindowLongPtr(window_handle, GWL_EXSTYLE) | WS_EX_TRANSPARENT);
            previousState = showMenu;
        }
        else if (showMenu) {
            SetCursorPos(savedMousePos.x, savedMousePos.y);

            SetWindowLongPtr(window_handle, GWL_EXSTYLE, GetWindowLongPtr(window_handle, GWL_EXSTYLE) & ~WS_EX_TRANSPARENT);
            ShowWindow(window_handle, SW_SHOW);

            AllowSetForegroundWindow(ASFW_ANY);

            HWND foregroundWnd = GetForegroundWindow();
            DWORD foregroundThread = GetWindowThreadProcessId(foregroundWnd, NULL);
            DWORD currentThread = GetCurrentThreadId();

            if (foregroundThread != currentThread) {
                AttachThreadInput(foregroundThread, currentThread, TRUE);
            }

            SetForegroundWindow(window_handle);
            SetActiveWindow(window_handle);

            if (foregroundThread != currentThread) {
                AttachThreadInput(foregroundThread, currentThread, FALSE);
            }

            //Settings::LoadHotkeys();
            Settings::LoadTeleports();
        }
        else {
            GetCursorPos(&savedMousePos);

            SetWindowLongPtr(window_handle, GWL_EXSTYLE, GetWindowLongPtr(window_handle, GWL_EXSTYLE) | WS_EX_TRANSPARENT);

            if (gameWindow) {
                GetWindowRect(gameWindow, &game_rect);
                SetForegroundWindow(gameWindow);
                SetActiveWindow(gameWindow);
            }
        }

        previousState = showMenu;
    }

    if (showMenu) {
        if (Settings::Menu::rainbowColor) {
            float time = (float)ImGui::GetTime() * Settings::Menu::rainbowSpeed;  
            float red = (sin(time * 0.5f) * 0.5f) + 0.5f; 
            float green = (sin(time * 0.5f + 2.0f) * 0.5f) + 0.5f; 
            float blue = (sin(time * 0.5f + 4.0f) * 0.5f) + 0.5f; 

            Settings::Menu::color.Value = ImVec4(red, green, blue, 1.0f);
        }


        ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight));
        ImGui::Begin("Main Window", nullptr, WindowFlags);
        {
            ImVec2 s = ImVec2(ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2, ImGui::GetWindowSize().y - ImGui::GetStyle().WindowPadding.y * 2);
            ImVec2 p = ImVec2(ImGui::GetWindowPos().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetWindowPos().y + ImGui::GetStyle().WindowPadding.y);
            auto drawList = ImGui::GetWindowDrawList();

            static float line_thickness = 2.0f;

            drawList->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + SidebarWidth, p.y + WindowHeight), Colors::SideBar, Rounding, ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersBottomLeft); // Sidebar

            drawList->AddRectFilled(ImVec2(p.x + SidebarWidth - LineThickness, p.y), ImVec2(p.x + SidebarWidth, p.y + WindowHeight), Colors::Line, Rounding); // Sidebar line

            drawList->AddRectFilled(ImVec2(p.x + SidebarWidth, p.y), ImVec2(p.x + WindowWidth, p.y + SubTabsHeight), Colors::Main, Rounding, ImDrawFlags_RoundCornersTopRight); // SubTabs

            drawList->AddRectFilled(ImVec2(p.x + SidebarWidth, p.y + SubTabsHeight - LineThickness), ImVec2(p.x + WindowWidth, p.y + SubTabsHeight), Colors::Line, Rounding); // SubTabs line

            drawList->AddRectFilled(ImVec2(p.x + SidebarWidth, p.y + SubTabsHeight), ImVec2(p.x + WindowWidth, p.y + WindowHeight), Colors::Main, Rounding, ImDrawFlags_RoundCornersBottomRight); // Main

            ImGui::BeginChild("Icon", ImVec2(SidebarWidth, SubTabsHeight));
            {
                ImVec2 childPos = ImGui::GetCursorScreenPos();

                //ImGui::PushFont(Fonts::RobotoMedium20);
                const char* label = " ";

                ImVec2 textSize = ImGui::CalcTextSize(label);

                ImVec2 textPos = ImVec2(
                    childPos.x + (195.0f - textSize.x) * 0.5f,
                    childPos.y + (50.0f - textSize.y) * 0.5f
                );

                drawList->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), label);

                //ImGui::PopFont();
            }
            ImGui::EndChild();

            ImGui::SameLine(0.0f, 0.0f);

            ImGui::BeginChild("SubTabs", ImVec2(0.0f, SubTabsHeight));
            {
                ImGui::BeginGroup();
                {
                    ImGui::PushFont(Fonts::InterSemiBold18);

                    switch (currentTab) {
                    case Tab::Visuals:
                        Menu::RenderVisualsSubTabs();
                        break;
                    case Tab::Settings:
                        Menu::RenderSettingsSubTabs();
                        break;
                    default:
                        break;
                    }

                    ImGui::PopFont();
                }
                ImGui::EndGroup();
            }
            ImGui::EndChild();


            ImGui::BeginChild("Sidebar", ImVec2(SidebarWidth, 0.0f)); {
                ImGui::BeginGroup();
                {
                    ImGui::PushFont(Fonts::InterSemiBold20);

                    if (ImGui::TabButton("Aimbot", ImVec2(SidebarWidth, SidebarHeight), currentTab == Tab::Aimbot)) currentTab = Tab::Aimbot;

                    if (ImGui::TabButton("Visuals", ImVec2(SidebarWidth, SidebarHeight), currentTab == Tab::Visuals)) currentTab = Tab::Visuals;

                    if (ImGui::TabButton("Inventory", ImVec2(SidebarWidth, SidebarHeight), currentTab == Tab::Inventory)) currentTab = Tab::Inventory;

                    if (ImGui::TabButton("Teleport", ImVec2(SidebarWidth, SidebarHeight), currentTab == Tab::Teleport)) currentTab = Tab::Teleport;

                    if (ImGui::TabButton("Misc", ImVec2(SidebarWidth, SidebarHeight), currentTab == Tab::Misc)) currentTab = Tab::Misc;

                    if (ImGui::TabButton("Player", ImVec2(SidebarWidth, SidebarHeight), currentTab == Tab::Player)) currentTab = Tab::Player;

                    if (ImGui::TabButton("Settings", ImVec2(SidebarWidth, SidebarHeight), currentTab == Tab::Settings)) currentTab = Tab::Settings;

                    if (ImGui::TabButton("Discord", ImVec2(SidebarWidth, SidebarHeight))) {
                        ShellExecuteA(NULL, "open", "https://discord.gg/55dTECaCKu", NULL, NULL, SW_SHOWNORMAL);
                        showMenu = false;
                    }

                    ImGui::PopFont();
                }
                ImGui::EndGroup();
            }
            ImGui::EndChild();

            ImGui::SameLine(0.0f, 0.0f);

            ImGui::SetCursorPos(ImVec2{ ImGui::GetCursorPosX() + 15.0f, ImGui::GetCursorPosY() + 15.0f });
            switch (currentTab) {
            case Tab::Aimbot:
                Menu::RenderAimbotTab();
                break;
            case Tab::Visuals:
                Menu::RenderVisualsTab();
                break;
            case Tab::Inventory:
                Menu::RenderInventoryTab();
                break;
            case Tab::Player:
                Menu::RenderPlayerTab();
                break;
            case Tab::Teleport:
                Menu::RenderTeleportTab();
                break;
            case Tab::Misc:
                Menu::RenderMiscTab();
                break;
            case Tab::Settings:
                Menu::RenderSettingsTab();
                break;
            default:
                break;
            }
        }
        ImGui::End();
    }
}

void Menu::RenderAimbotTab() {
    ImGui::BeginChild("LeftColumn", ImVec2(330, 0));
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, Colors::Window.Value);
        ImGui::BeginChild("lWindow1", ImVec2(0, 500));
        ImGui::PopStyleColor();
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7.5);

            ImGui::BeginGroup();
            {
                ImGui::CustomCheckbox("Enable Aimbot", &Settings::Aimbot::enabled);

  
                ImGui::CustomCheckbox("CheckFOV", &Settings::Aimbot::checkFOV);
                ImGui::CustomCheckbox("DrawFOV", &Settings::Aimbot::drawFOV);

                ImGui::Spacing();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
                ImGui::Text("Hotkey");
                ImGui::SameLine(ImGui::GetContentRegionAvail().x / 1.35);
                if (ImGui::ImHotkey("Hotkey##Aimbot", &Settings::Aimbot::hotkey, ImVec2(75, 20))) {
                    Settings::SaveHotkeys();
                }

                ImGui::Spacing();
                ImGui::Spacing();

                //ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
                //ImGui::Text("Hotkey");

                //ImGui::SameLine(ImGui::GetContentRegionAvail().x - 140);
                //ImHotkey("Aimbot Hotkey", &Settings::Aimbot::hotkey, ImVec2(60, 20));

                ImGui::CustomSliderFloat("Smooth", &Settings::Aimbot::smooth, 0.1, 1.0, "%.2f");
                ImGui::Spacing();
                ImGui::CustomSliderFloat("FOV##aimbot", &Settings::Aimbot::fov, 10.0f, 180.0f, "%.1f");
                ImGui::Spacing();
                ImGui::CustomSliderDouble("Max Distance##aimbot", &Settings::Aimbot::maxDistance, 0.0, 150.0, "%.0lf");
                ImGui::Spacing();
                ImGui::CustomSliderInt("Trigger Shoot Delay", &Settings::Aimbot::triggerShootDelay, 0, 500);

                ImGui::Spacing();
                ImGui::Spacing();

                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
                ImGui::BezierCubicCurve("BezierCubicCurve", &Settings::Aimbot::p2, &Settings::Aimbot::p3);
            }
            ImGui::EndGroup();
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();

    ImGui::SameLine();
    
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15.0f);
    ImGui::BeginChild("RightColumn", ImVec2(330, 0));
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, Colors::Window.Value);
        ImGui::BeginChild("rWindow1", ImVec2(0, 300));
        ImGui::PopStyleColor();
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
            ImGui::BeginGroup();
            {

            }
            ImGui::EndGroup();
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();
}

auto VisualsSubTab = SubTabs::Visuals;
void Menu::RenderVisualsSubTabs() {
    if (ImGui::TabButton2("Visuals", ImVec2(150, SubTabsHeight), VisualsSubTab == SubTabs::Visuals)) VisualsSubTab = SubTabs::Visuals;
    ImGui::SameLine();
    if (ImGui::TabButton2("Colors", ImVec2(150, SubTabsHeight), VisualsSubTab == SubTabs::Colors)) VisualsSubTab = SubTabs::Colors;
}

static SnapSpot nameCurrentSpot = { SpotGroup::Top, 0};
static SnapSpot distanceCurrentSpot = { SpotGroup::Bottom, 0 };

static std::unordered_set<SnapSpot> occupiedSpots = { nameCurrentSpot, distanceCurrentSpot };


auto color_group = ItemGroup::Weapons;
void Menu::RenderVisualsTab() {
    if (VisualsSubTab == SubTabs::Visuals) {
        ImGui::BeginChild("LeftColumn", ImVec2(330, 0));
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, Colors::Window.Value);
            ImGui::BeginChild("lWindow1", ImVec2(0, 520));
            {
                ImGui::PopStyleColor();
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7.5);
                ImGui::BeginGroup();
                {
                    ImGui::CustomCheckbox("Enable Players ESP", &Settings::ESP::Players::enabled);

                    ImGui::CustomCheckbox("Player", &Settings::ESP::Players::player);
                    ImGui::CustomCheckbox("Ghost", &Settings::ESP::Players::ghost);
                    ImGui::CustomCheckbox("Dead Body", &Settings::ESP::Players::deadBody);
                    //ImGui::CustomCheckbox("Skeleton", &Settings::ESP::Players::skeleton);
                    ImGui::CustomCheckbox("Dissidents", &Settings::ESP::Players::dissidents);
                    ImGui::CustomCheckbox("2D Box##players", &Settings::ESP::Players::box);
                    ImGui::CustomCheckbox("Name##players", &Settings::ESP::Players::name);
                    ImGui::CustomCheckbox("Distance##players", &Settings::ESP::Players::distance);
                    ImGui::CustomCheckbox("Snapline##players", &Settings::ESP::Players::snapline);
                    ImGui::CustomCheckbox("Inventory##players", &Settings::ESP::Players::inventory);
                    //ImGui::CustomCheckbox("Preview##players", &Settings::ESP::Players::preview);
                    //ImGui::CustomCheckbox("Look Direction##players", &Settings::ESP::Players::lookDirection);
                    if (Settings::ESP::Players::preview) {

                    }

                    ImGui::Spacing();
                    ImGui::Spacing();

                    ImGui::CustomSliderDouble("Max Distance##players", &Settings::ESP::Players::maxDistance, 0.0, 150.0, "%.0lf");

                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
                    ImGui::ImVec2Editor("ImVec2Editor", &Settings::ESP::Players::snaplinePos);
                }
                ImGui::EndGroup();
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15.0f);
        ImGui::BeginChild("RightColumn", ImVec2(330, 0));
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, Colors::Window.Value);
            ImGui::BeginChild("rWindow1", ImVec2(0, 520));
            {
                ImGui::PopStyleColor();
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7.5);
                ImGui::BeginGroup();
                {
                    ImGui::CustomCheckbox("Enable Items ESP", &Settings::ESP::Items::enabled);

                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 2));

                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();

                    ImGui::SetNextItemWidth(240);
                    ImGui::MultiCombo("Weapons", Settings::ESP::Items::weapons_map);

                    ImGui::Spacing();
                    ImGui::Spacing();

                    ImGui::SetNextItemWidth(240);
                    ImGui::MultiCombo("Primary", Settings::ESP::Items::primary_map);

                    ImGui::Spacing();
                    ImGui::Spacing();

                    ImGui::SetNextItemWidth(240);
                    ImGui::MultiCombo("Secondary", Settings::ESP::Items::secondary_map);

                    ImGui::PopStyleVar();

                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();

                    ImGui::CustomCheckbox("Name##Items", &Settings::ESP::Items::name);
                    ImGui::CustomCheckbox("State##Items", &Settings::ESP::Items::state);
                    ImGui::CustomCheckbox("Distance##Items", &Settings::ESP::Items::distance);
                    // ImGui::CustomCheckbox("Snapline##Items", &Settings::ESP::Items::snapline);

                    ImGui::Spacing();
                    ImGui::Spacing();

                    ImGui::CustomSliderDouble("Max Distance##Items", &Settings::ESP::Items::maxDistance, 0.0, 150.0, "%.0lf");
                }
                ImGui::EndGroup();
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();

        if (Settings::ESP::Players::preview) {
            ImVec2 previewPos = ImVec2(
                ImGui::GetWindowPos().x + ImGui::GetWindowSize().x + 10,
                ImGui::GetWindowPos().y
            );

            ImGui::SetNextWindowPos(previewPos, ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(400, WindowHeight), ImGuiCond_Always);

            ImGui::Begin("ESP Preview", &Settings::ESP::Players::preview, WindowFlags);
            {
                static float prevTextOffset = Settings::ESP::Players::textOffset;
                static float baseOffset = Settings::ESP::Players::baseOffset;

                ImGui::Spacing();
                ImGui::Spacing();

                ImDrawList* drawList = ImGui::GetWindowDrawList();
                ImVec2 canvasPos = ImGui::GetCursorScreenPos();
                ImVec2 canvasSize = ImGui::GetContentRegionAvail();

                drawList->AddRectFilled(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), Colors::Window, Rounding);

                ImGui::CustomSliderFloat("Text Offset", &Settings::ESP::Players::textOffset, 0.0f, 50.0f, "%.0f");
                bool offsetChanged = (prevTextOffset != Settings::ESP::Players::textOffset);
                prevTextOffset = Settings::ESP::Players::textOffset;

                if (Settings::ESP::Players::enabled) {
                    canvasPos = ImGui::GetCursorScreenPos();
                    canvasSize = ImGui::GetContentRegionAvail();

                    ImVec2 center = ImVec2(canvasPos.x + canvasSize.x * 0.5f, canvasPos.y + canvasSize.y * 0.5f);
                    float boxHeight = 325.0f;
                    float boxWidth = boxHeight * 0.6f;
                    ImVec2 boxMin = ImVec2(center.x - boxWidth * 0.5f, center.y - boxHeight * 0.5f);
                    ImVec2 boxMax = ImVec2(center.x + boxWidth * 0.5f, center.y + boxHeight * 0.5f);

                    if (Settings::ESP::Players::box) {
                        drawList->AddRect(
                            boxMin, boxMax,
                            ImColor(Settings::ESP::Colors::Employee.Value),
                            0.0f, 0, 1.0f
                        );
                    }

                    const char* demoName = "Employee";
                    const char* demoDistance = "12.5m";
                    ImVec2 nameTextSize = ImGui::CalcTextSize(demoName);
                    ImVec2 distanceTextSize = ImGui::CalcTextSize(demoDistance);

                    std::unordered_map<SnapSpot, ImVec2> snapPositions = {
                        { {SpotGroup::Top, 0},    ImVec2(center.x, boxMin.y - (baseOffset + Settings::ESP::Players::textOffset + (0 * 30))) },
                        { {SpotGroup::Top, 1},    ImVec2(center.x, boxMin.y - (baseOffset + Settings::ESP::Players::textOffset + (1 * 30))) },
                        { {SpotGroup::Top, 2},    ImVec2(center.x, boxMin.y - (baseOffset + Settings::ESP::Players::textOffset + (2 * 30))) },

                        { {SpotGroup::Bottom, 0}, ImVec2(center.x, boxMax.y + (baseOffset + Settings::ESP::Players::textOffset + (0 * 30))) },
                        { {SpotGroup::Bottom, 1}, ImVec2(center.x, boxMax.y + (baseOffset + Settings::ESP::Players::textOffset + (1 * 30))) },
                        { {SpotGroup::Bottom, 2}, ImVec2(center.x, boxMax.y + (baseOffset + Settings::ESP::Players::textOffset + (2 * 30))) },
                    };

                    static bool initializedDefaults = false;
                    if (!initializedDefaults || offsetChanged) {
                        Settings::ESP::Players::nameLocalPos = snapPositions[nameCurrentSpot] - canvasPos;
                        Settings::ESP::Players::distanceLocalPos = snapPositions[distanceCurrentSpot] - canvasPos;
                        initializedDefaults = true;
                    }

                    if (Settings::ESP::Players::name || Settings::ESP::Players::distance) {
                        for (const auto& [spot, pos] : snapPositions) {
                            bool occupied = occupiedSpots.count(spot) > 0;
                            bool isHighlighted = false;

                            if (isHighlighted) {
                                drawList->AddCircleFilled(pos, 5.0f, IM_COL32(255, 255, 0, 200));
                            }
                            else {
                                ImU32 color = occupied ? IM_COL32(255, 50, 50, 150) : IM_COL32(200, 200, 200, 150);
                                drawList->AddCircle(pos, 3.0f, color);
                            }
                        }
                    }

                    // Unified draggable text handler
                    auto DrawDraggableText = [&](const char* id, ImVec2& position, SnapSpot& currentSpot, const char* text, const ImVec2& textSize, ImU32 color) {
                            static std::unordered_map<const char*, bool> isDragging;
                            static std::unordered_map<const char*, ImVec2> dragStartPos;
                            static std::unordered_map<const char*, ImVec2> startLocalPos;
                            static std::unordered_map<const char*, SnapSpot> highlightedSpot;

                            ImVec2 screenPos = canvasPos + position;
                            ImVec2 textPos = ImVec2(screenPos.x - textSize.x * 0.5f, screenPos.y - textSize.y * 0.5f);

                            ImGui::SetCursorScreenPos(textPos);
                            ImGui::InvisibleButton(id, textSize);

                            if (ImGui::IsItemActivated()) {
                                isDragging[id] = true;
                                dragStartPos[id] = ImGui::GetMousePos();
                                startLocalPos[id] = position;
                            }

                            if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                                ImVec2 delta = ImGui::GetMousePos() - dragStartPos[id];
                                position = ImVec2(startLocalPos[id].x + delta.x, startLocalPos[id].y + delta.y);

                                ImVec2 currentScreenPos = canvasPos + position;
                                float minDistance = FLT_MAX;
                                for (const auto& [spot, snapPos] : snapPositions) {
                                    float distance = ImLengthSqr(ImVec2(currentScreenPos.x - snapPos.x, currentScreenPos.y - snapPos.y));
                                    if (distance < minDistance) {
                                        minDistance = distance;
                                        highlightedSpot[id] = spot;
                                    }
                                }
                            }

                            if (ImGui::IsItemDeactivated()) {
                                isDragging[id] = false;
                                ImVec2 currentScreenPos = canvasPos + position;
                                float minDistance = FLT_MAX;
                                SnapSpot closestSpot = currentSpot;

                                occupiedSpots.erase(currentSpot);

                                for (const auto& [spot, snapPos] : snapPositions) {
                                    if (occupiedSpots.count(spot) && spot != currentSpot)
                                        continue;

                                    float distance = ImLengthSqr(ImVec2(currentScreenPos.x - snapPos.x, currentScreenPos.y - snapPos.y));
                                    if (distance < minDistance) {
                                        minDistance = distance;
                                        closestSpot = spot;
                                    }
                                }
                                position = snapPositions[closestSpot] - canvasPos;
                                currentSpot = closestSpot;

                                occupiedSpots.insert(currentSpot);
                            }

                            drawList->AddText(textPos, color, text);
                    };

                    ImU32 textColor = ImColor(Settings::ESP::Colors::Employee.Value);
                    if (Settings::ESP::Players::name) {
                        DrawDraggableText("##DragName",
                            Settings::ESP::Players::nameLocalPos,
                            nameCurrentSpot,
                            demoName,
                            nameTextSize,
                            textColor
                        );
                    }

                    if (Settings::ESP::Players::distance) {
                        DrawDraggableText("##DragDistance",
                            Settings::ESP::Players::distanceLocalPos,
                            distanceCurrentSpot,
                            demoDistance,
                            distanceTextSize,
                            textColor
                        );
                    }

                    ImVec2 windowSize = ImGui::GetWindowSize();

                    ImGui::PushFont(Fonts::InterSemiBold40);

                    ImGui::SetCursorPos(ImVec2(
                        (windowSize.x - ImGui::CalcTextSize("WIP").x) * 0.5f,
                        (windowSize.y - ImGui::GetFontSize()) * 0.5f
                    ));

                    ImGui::Text("WIP");
                    ImGui::PopFont();
                }
            }
            ImGui::End();
        }
    }
    else {
        ImGuiColorEditFlags flags =
            ImGuiColorEditFlags_AlphaBar
            | ImGuiColorEditFlags_NoInputs
            | ImGuiColorEditFlags_NoSidePreview
            | ImGuiColorEditFlags_NoTooltip
            | ImGuiColorEditFlags_PickerHueBar
            | ImGuiColorEditFlags_NoOptions;

        ImGui::BeginChild("LeftColumn", ImVec2(330, 0));
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, Colors::Window.Value);
            ImGui::BeginChild("lWindow1", ImVec2(0, 500));
            {
                ImGui::PopStyleColor();
                ImGui::SetCursorPos(ImVec2{ ImGui::GetCursorPosX() + 10.0f, ImGui::GetCursorPosY() + 10.0f });
                ImGui::BeginGroup();
                {
                    ImGui::ColorEdit4("Dissident", (float*)&Settings::ESP::Colors::Dissident.Value, flags);
                    ImGui::ColorEdit4("Dissident Ghost", (float*)&Settings::ESP::Colors::DissidentGhost.Value, flags);
                    ImGui::ColorEdit4("Dissident Body", (float*)&Settings::ESP::Colors::DissidentBody.Value, flags);

                    ImGui::Spacing();

                    ImGui::ColorEdit4("Employee", (float*)&Settings::ESP::Colors::Employee.Value, flags);
                    ImGui::ColorEdit4("Employee Ghost", (float*)&Settings::ESP::Colors::EmployeeGhost.Value, flags);
                    ImGui::ColorEdit4("Employee Body", (float*)&Settings::ESP::Colors::EmployeeBody.Value, flags);
                }
                ImGui::EndGroup();
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15.0f);
        ImGui::BeginChild("RightColumn", ImVec2(330, 0));
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, Colors::Window.Value);
            ImGui::BeginChild("rWindow1", ImVec2(0, 500));
            {

                auto xAvail = ImGui::GetContentRegionAvail().x;

                ImGui::PushFont(Fonts::InterMedium18);
                if (ImGui::TabButton2("Weapons##Group", ImVec2(xAvail / 3, 40), color_group == ItemGroup::Weapons)) color_group = ItemGroup::Weapons;
                ImGui::SameLine(0.0f, 0.0f);
                if (ImGui::TabButton2("Primary##Group", ImVec2(xAvail / 3, 40), color_group == ItemGroup::Primary)) color_group = ItemGroup::Primary;
                ImGui::SameLine(0.0f, 0.0f);
                if (ImGui::TabButton2("Secondary##Group", ImVec2(xAvail / 3, 40), color_group == ItemGroup::Secondary)) color_group = ItemGroup::Secondary;
                ImGui::PopFont();

                ImGui::PopStyleColor();
                ImGui::SetCursorPos(ImVec2{ ImGui::GetCursorPosX() + 10.0f, ImGui::GetCursorPosY() + 10.0f });
                ImGui::BeginGroup();
                {
                    ImGui::Spacing();
                    ImGui::Spacing();

                    if (color_group == ItemGroup::Weapons) {
                        ImGui::BeginGroup();
                        {
                            ImGui::ColorEdit4("Knife", (float*)&Settings::ESP::Colors::Knife, flags);
                            ImGui::ColorEdit4("Revolver", (float*)&Settings::ESP::Colors::Revolver, flags);
                            ImGui::ColorEdit4("Pistol", (float*)&Settings::ESP::Colors::Pistol, flags);
                            ImGui::ColorEdit4("Detonator", (float*)&Settings::ESP::Colors::Detonator, flags);
                            ImGui::ColorEdit4("C4", (float*)&Settings::ESP::Colors::C4, flags);
                            ImGui::ColorEdit4("Shorty", (float*)&Settings::ESP::Colors::Shorty, flags);
                            ImGui::ColorEdit4("Shotgun", (float*)&Settings::ESP::Colors::Shotgun, flags);
                            ImGui::ColorEdit4("Rifle", (float*)&Settings::ESP::Colors::Rifle, flags);
                            ImGui::ColorEdit4("SMG", (float*)&Settings::ESP::Colors::SMG, flags);
                            ImGui::ColorEdit4("Default Gun", (float*)&Settings::ESP::Colors::DefaultGun, flags);
                        }
                        ImGui::EndGroup();
                    }
                    else if (color_group == ItemGroup::Primary) {
                        ImGui::BeginGroup();
                        {
                            ImGui::ColorEdit4("Gaz Bottle", (float*)&Settings::ESP::Colors::GazBottle, flags);
                            ImGui::ColorEdit4("Vent Filter", (float*)&Settings::ESP::Colors::VentFilter, flags);
                            ImGui::ColorEdit4("Rice", (float*)&Settings::ESP::Colors::Rice, flags);
                            ImGui::ColorEdit4("Package", (float*)&Settings::ESP::Colors::Package, flags);
                            ImGui::ColorEdit4("Pizzushi", (float*)&Settings::ESP::Colors::Pizzushi, flags);
                            ImGui::ColorEdit4("Defibrillator", (float*)&Settings::ESP::Colors::Defibrillator, flags);
                        }
                        ImGui::EndGroup();
                    }
                    else if (color_group == ItemGroup::Secondary) {
                        ImGui::BeginGroup();
                        {
                            ImGui::ColorEdit4("Battery", (float*)&Settings::ESP::Colors::Battery, flags);
                            ImGui::ColorEdit4("Screw Driver", (float*)&Settings::ESP::Colors::ScrewDriver, flags);
                            ImGui::ColorEdit4("Container", (float*)&Settings::ESP::Colors::Container, flags);
                            ImGui::ColorEdit4("Sample", (float*)&Settings::ESP::Colors::Sample, flags);
                            ImGui::ColorEdit4("Cassette", (float*)&Settings::ESP::Colors::Cassette, flags);
                            ImGui::ColorEdit4("Fuse", (float*)&Settings::ESP::Colors::Fuse, flags);
                            ImGui::ColorEdit4("Fish", (float*)&Settings::ESP::Colors::Fish, flags);
                            ImGui::ColorEdit4("Hard Drive", (float*)&Settings::ESP::Colors::HardDrive, flags);
                            ImGui::ColorEdit4("Access Card", (float*)&Settings::ESP::Colors::AccessCard, flags);
                            ImGui::ColorEdit4("Default Melee", (float*)&Settings::ESP::Colors::DefaultMelee, flags);
                        }
                        ImGui::EndGroup();
                    }
                }
                ImGui::EndGroup();

            }
            ImGui::EndChild();
        }
        ImGui::EndChild();
    }
}

auto inventory_slot = InventorySlot::Hand;
auto items_group = ItemGroup::Weapons;

void Menu::RenderInventoryTab() {

    const auto width = 320;
    const auto height = 25;
    const float margin = 10.0f;

    ImGui::BeginChild("LeftColumn", ImVec2(width, 0));
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, Colors::Window.Value);

        ImGui::BeginChild("lWindow1", ImVec2(0, 475));
        {   
            auto xAvail = ImGui::GetContentRegionAvail().x;

            ImGui::PushFont(Fonts::InterMedium18);
            if (ImGui::TabButton2("Weapons##Group", ImVec2(xAvail / 3, 40), items_group == ItemGroup::Weapons)) items_group = ItemGroup::Weapons;
            ImGui::SameLine(0.0f, 0.0f);
            if (ImGui::TabButton2("Primary##Group", ImVec2(xAvail / 3, 40), items_group == ItemGroup::Primary)) items_group = ItemGroup::Primary;
            ImGui::SameLine(0.0f, 0.0f);
            if (ImGui::TabButton2("Secondary##Group", ImVec2(xAvail / 3, 40), items_group == ItemGroup::Secondary)) items_group = ItemGroup::Secondary;
            ImGui::PopFont();

            ImGui::Spacing();
            ImGui::Spacing();

            if (items_group == ItemGroup::Weapons) {
                ImGui::BeginGroup();
                {
                    ImGui::InventoryButton("KNIFE", ImVec2(width, height), "KNIFE");
                    ImGui::InventoryButton("REVOLVER", ImVec2(width, height), "REVOLVER");
                    ImGui::InventoryButton("PISTOL", ImVec2(width, height), "PISTOL");
                    ImGui::InventoryButton("DETONATOR", ImVec2(width, height), "DETONATOR");
                    ImGui::InventoryButton("C4", ImVec2(width, height), "C4");
                    ImGui::InventoryButton("SHORTY", ImVec2(width, height), "SHORTY");
                    ImGui::InventoryButton("SHOTGUN", ImVec2(width, height), "SHOTGUN");
                    ImGui::InventoryButton("RIFLE", ImVec2(width, height), "RIFLE");
                    ImGui::InventoryButton("SMG", ImVec2(width, height), "SMG");
                    ImGui::InventoryButton("DEFAULT GUN", ImVec2(width, height), "DEFAULT GUN");
                }
                ImGui::EndGroup();
            }
            else if (items_group == ItemGroup::Primary) {
                ImGui::BeginGroup();
                {
                    ImGui::InventoryButton("GAZ BOTTLE", ImVec2(width, height), "GAZ BOTTLE");
                    ImGui::InventoryButton("VENT FILTER", ImVec2(width, height), "VENT FILTER");
                    ImGui::InventoryButton("RICE", ImVec2(width, height), "RICE");
                    ImGui::InventoryButton("PACKAGE", ImVec2(width, height), "PACKAGE");
                    ImGui::InventoryButton("PIZZUSHI", ImVec2(width, height), "PIZZUSHI");
                    ImGui::InventoryButton("DEFIBRILLATOR", ImVec2(width, height), "DEFIBRILLATOR");
                    ImGui::InventoryButton("MACHINE PART A", ImVec2(width, height), "MACHINE PART A");
                    ImGui::InventoryButton("MACHINE PART B", ImVec2(width, height), "MACHINE PART B");
                    ImGui::InventoryButton("MACHINE PART C", ImVec2(width, height), "MACHINE PART C");
                    ImGui::InventoryButton("MACHINE PART D", ImVec2(width, height), "MACHINE PART D");
                }
                ImGui::EndGroup();
            }
            else if (items_group == ItemGroup::Secondary) {
                ImGui::BeginGroup();
                {
                    ImGui::InventoryButton("BATTERY", ImVec2(width, height), "BATTERY");
                    ImGui::InventoryButton("SCREW DRIVER", ImVec2(width, height), "SCREW DRIVER");
                    ImGui::InventoryButton("CONTAINER", ImVec2(width, height), "CONTAINER");
                    ImGui::InventoryButton("SAMPLE", ImVec2(width, height), "SAMPLE");
                    ImGui::InventoryButton("CASSETTE", ImVec2(width, height), "CASSETTE");
                    ImGui::InventoryButton("FUSE", ImVec2(width, height), "FUSE");
                    ImGui::InventoryButton("FISH", ImVec2(width, height), "FISH");
                    ImGui::InventoryButton("HARD DRIVE", ImVec2(width, height), "HARD DRIVE");
                    ImGui::InventoryButton("ACCESS CARD", ImVec2(width, height), "ACCESS CARD");
                    ImGui::InventoryButton("EGG", ImVec2(width, height), "EGG");
                    ImGui::InventoryButton("DEFAULT MELEE", ImVec2(width, height), "DEFAULT MELEE");
                }
                ImGui::EndGroup();
            }

            ImGui::Dummy(ImVec2(0, ImGui::GetContentRegionAvail().y - height * 3 - 5 * 3));

            if (ImGui::CustomButton("Save Weapon Config", ImVec2(width, height))) {
                ImGui::OpenPopup("SaveWeaponConfig");
            }

            if (ImGui::IsPopupOpen("SaveWeaponConfig")) {
                ImVec2 window_pos = ImGui::GetWindowPos();  
                ImVec2 window_size = ImGui::GetWindowSize(); 

                ImVec2 popup_size = ImVec2(300, ImGui::GetItemRectSize().y * 2 + height);
                ImVec2 popup_pos = ImVec2(
                    window_pos.x + (window_size.x - popup_size.x) * 0.5f,  
                    window_pos.y + (window_size.y - popup_size.y) * 0.5f  
                );

                ImGui::SetNextWindowPos(popup_pos, ImGuiCond_Appearing);
                ImGui::SetNextWindowSize(popup_size);
            }

            if (ImGui::BeginPopup("SaveWeaponConfig")) {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, (height - ImGui::GetFontSize()) / 2.0f));
                ImGui::PushStyleColor(ImGuiCol_FrameBg, Colors::Window.Value);
                ImGui::PushStyleColor(ImGuiCol_Button, Colors::Window.Value);

                static char config_name[128] = "";

                ImGui::SetCursorPos(ImVec2{ ImGui::GetCursorPosX() + margin, ImGui::GetCursorPosY() + margin });

                ImGui::BeginGroup();
                {
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - margin);
                    ImGui::InputText("##WeaponConfig", config_name, IM_ARRAYSIZE(config_name));

                    if (ImGui::CustomButton("Confirm", ImVec2(ImGui::GetContentRegionAvail().x - margin, height))) {
                        if (strlen(config_name) != 0) {
                            u_data_item* item = nullptr;

                            if (inventory_slot == InventorySlot::Hand) {
                                item = local_mec->get_hand_item();
                            }
                            else if (inventory_slot == InventorySlot::Bag) {
                                item = local_mec->get_bag_item();
                            }

                            if (item) {
                                auto item_name = item->get_name().read_string();
                                auto gun_data = (u_data_gun*)item;
                                auto melee_data = (u_data_melee*)item;

                                ItemProperties item_stats{};

                                if (util::get_name_from_fname(item->class_private()->fname_index()).find("Data_Melee_C") != std::string::npos) {
                                    auto melee_type = melee_data->get_melee_type();

                                    item_stats.name = item_name;
                                    item_stats.frontal_damage_health = melee_type->get_heal();
                                    item_stats.frontal_damage_stamina = melee_type->get_stamina();
                                    item_stats.frontal_damage_slow = melee_type->get_stun();

                                    item_stats.back_damage_health = melee_type->get_crit_heal();
                                    item_stats.back_damage_stamina = melee_type->get_crit_stamina();
                                    item_stats.back_damage_slow = melee_type->get_crit_stun();

                                    item_stats.attack_range = melee_type->get_range();
                                    item_stats.attack_speed = melee_type->get_recover_time();
                                    item_stats.stamina_cost = melee_type->get_cost();
                                    item_stats.cast_time = melee_type->get_cast_time();

                                    item_stats.can_inventory = gun_data->get_can_inventory();
                                }
                                else {
                                    item_stats.is_melee = false;

                                    item_stats.name = item_name;
                                    item_stats.body_damage_health = gun_data->get_damage();
                                    item_stats.body_damage_stamina = gun_data->get_stamina_damage();
                                    item_stats.body_damage_slow = gun_data->get_stun();

                                    item_stats.head_damage_health = gun_data->get_crit();
                                    item_stats.head_damage_stamina = gun_data->get_crit_stamina();

                                    item_stats.max_ammo = gun_data->get_capacity();
                                    item_stats.fire_rate = gun_data->get_fire_rate();
                                    item_stats.stamina_cost = gun_data->get_stamina_usage();

                                    item_stats.impact_type = gun_data->get_impact_type();
                                    item_stats.pattern_type = gun_data->get_pattern_type();

                                    item_stats.auto_fire = gun_data->get_auto_fire();
                                    item_stats.can_inventory = gun_data->get_can_inventory();
                                }

                                std::string filepath = "Configs/Weapon/" + std::string("[" + item_stats.name + "] " + config_name) + ".json";
                                Settings::SaveWeaponConfig(filepath, item_stats);
                            }
                        }
                        memset(config_name, 0, sizeof(config_name));  // Clear config_name here before closing the popup
                        ImGui::CloseCurrentPopup();
                    }
                }
                ImGui::EndGroup();

                ImGui::PopStyleVar();
                ImGui::PopStyleColor(2); 
                ImGui::EndPopup();
            }

            if (ImGui::CustomButton("Load Weapon Config", ImVec2(width, height))) {
                ImGui::OpenPopup("LoadWeaponConfig");
            }

            if (ImGui::IsPopupOpen("LoadWeaponConfig")) {
                ImVec2 window_pos = ImGui::GetWindowPos();
                ImVec2 window_size = ImGui::GetWindowSize();

                ImVec2 popup_size = ImVec2(300, ImGui::GetItemRectSize().y * 15 + height - 10);
                ImVec2 popup_pos = ImVec2(
                    window_pos.x + (window_size.x - popup_size.x) * 0.5f,
                    window_pos.y + (window_size.y - popup_size.y) * 0.5f
                );

                ImGui::SetNextWindowPos(popup_pos, ImGuiCond_Appearing);
                ImGui::SetNextWindowSize(popup_size);
            }

            ImGui::PushStyleColor(ImGuiCol_Button, Colors::Window.Value);

            if (ImGui::BeginPopup("LoadWeaponConfig")) {
                static char config_name[128] = "";
                static std::string config_to_delete;

                ImGui::SetCursorPos(ImVec2{ ImGui::GetCursorPosX() + margin, ImGui::GetCursorPosY() + margin });

                ImGui::BeginGroup();
                {
                    std::filesystem::path configDir = "Configs/Weapon/";

                    if (!std::filesystem::exists(configDir)) {
                        std::filesystem::create_directories(configDir);
                        std::cout << "Configs/Weapon directory created." << std::endl;
                    }

                    std::vector<std::string> configs = Settings::GetSaveFiles(configDir);

                    for (const auto& config : configs) {
                        std::string display = Settings::StringConfigName(config);

                        if (ImGui::CustomButton(display.c_str(), ImVec2(ImGui::GetContentRegionAvail().x - margin, height))) {
                            std::string filepath = configDir.string() + config;
                            Settings::LoadWeaponConfig(filepath);
                        }

                        if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
                            config_to_delete = config;
                            ImGui::OpenPopup("DeleteWeaponConfig");
                        }
                    }

                    if (ImGui::BeginPopup("DeleteWeaponConfig")) {
                        if (ImGui::Button("Delete", ImVec2(100, 33))) {
                            std::string filepath = configDir.string() + config_to_delete;
                            std::filesystem::remove(filepath);
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                }
                ImGui::EndGroup();
                ImGui::EndPopup();
            }

            ImGui::PopStyleColor(1);



            if (ImGui::CustomButton("Reset all stats", ImVec2(width, height))) {
                std::vector<std::string> unique_item_names = {
                    "REVOLVER",
                    "SHORTY",
                    "PISTOL",
                    "SHOTGUN",
                    "RIFLE",
                    "SMG",
                    "GAZ BOTTLE",
                    "BATTERY",
                    "SCREW DRIVER",
                    "KNIFE",
                    "FISH",
                    "DEFAULT GUN",
                    "DEFIBRILLATOR",
                    "MACHINE PART A",
                    "MACHINE PART B",
                    "MACHINE PART C",
                    "MACHINE PART D",
                };

                for (const auto& name : unique_item_names) {
                    auto item = unique_item_data[name];

                    if (item) {
                        auto gun_data = (u_data_gun*)item;
                        auto melee_data = (u_data_melee*)item;

                        if (util::get_name_from_fname(item->class_private()->fname_index()).find("Data_Melee_C") != std::string::npos) {
                            auto melee_type = melee_data->get_melee_type();
                            auto item_stats = GetItemProperties(name);

                            melee_type->set_heal(item_stats.frontal_damage_health);
                            melee_type->set_stamina(item_stats.frontal_damage_stamina);
                            melee_type->set_stun(item_stats.frontal_damage_slow);

                            melee_type->set_crit_heal(item_stats.back_damage_health);
                            melee_type->set_crit_stamina(item_stats.back_damage_stamina);
                            melee_type->set_crit_stun(item_stats.back_damage_slow);

                            melee_type->set_range(item_stats.attack_range);
                            melee_type->set_recover_time(item_stats.attack_speed);
                            melee_type->set_cost(item_stats.stamina_cost);
                            melee_type->set_cast_time(item_stats.cast_time);

                            gun_data->set_can_inventory(item_stats.can_inventory);
                        }
                        else {
                            auto item_stats = GetItemProperties(name);

                            gun_data->set_damage(item_stats.body_damage_health);
                            gun_data->set_stamina_damage(item_stats.body_damage_stamina);
                            gun_data->set_stun(item_stats.body_damage_slow);

                            gun_data->set_crit(item_stats.head_damage_health);
                            gun_data->set_crit_stamina(item_stats.head_damage_stamina);

                            gun_data->set_capacity(item_stats.max_ammo);
                            gun_data->set_fire_rate(item_stats.fire_rate);
                            gun_data->set_stamina_usage(item_stats.stamina_cost);

                            gun_data->set_impact_type(item_stats.impact_type);
                            gun_data->set_pattern_type(item_stats.pattern_type);

                            gun_data->set_auto_fire(item_stats.auto_fire);
                            gun_data->set_can_inventory(item_stats.can_inventory);
                        }
                    }
                }
            }

            //if (ImGui::CustomButton("Log", ImVec2(width, height))) {
            //    auto item = local_mec->get_hand_item();
            //    if (item) {
            //        auto gun_data = (u_data_gun*)item;
            //        auto melee_data = (u_data_melee*)item;

            //        if (util::get_name_from_fname(item->class_private()->fname_index()).find("Data_Melee_C") != std::string::npos) {
            //            auto melee_type = melee_data->get_melee_type();

            //            std::cout << "===== Back Damage =====" << std::endl;
            //            std::cout << "Health: " << melee_type->get_heal() << std::endl;
            //            std::cout << "Stamina: " << melee_type->get_stamina() << std::endl;
            //            std::cout << "Slow: " << melee_type->get_stun() << std::endl;

            //            std::cout << std::endl;

            //            std::cout << "===== Frontal Damage =====" << std::endl;
            //            std::cout << "Health: " << melee_type->get_crit_heal() << std::endl;
            //            std::cout << "Stamina: " << melee_type->get_crit_stamina() << std::endl;
            //            std::cout << "Slow: " << melee_type->get_crit_stun() << std::endl;

            //            std::cout << std::endl;

            //            std::cout << "===== Other =====" << std::endl;
            //            std::cout << "Attack Range: " << melee_type->get_range() << std::endl;
            //            std::cout << "Attack Speed: " << melee_type->get_recover_time() << std::endl;

            //            std::cout << "Stamina Cost: " << melee_type->get_cost() << std::endl;
            //            std::cout << "Cast Time: " << melee_type->get_cast_time() << std::endl;

            //            std::cout << "Can Inventory: " << std::boolalpha << gun_data->get_can_inventory() << std::endl;
            //        }
            //        else {
            //            std::cout << "===== Body Damage =====" << std::endl;
            //            std::cout << "Health: " << gun_data->get_damage() << std::endl;
            //            std::cout << "Stamina: " << gun_data->get_stamina_damage() << std::endl;
            //            std::cout << "Slow: " << gun_data->get_stun() << std::endl;

            //            std::cout << std::endl;

            //            std::cout << "===== Head Damage =====" << std::endl;
            //            std::cout << "Health: " << gun_data->get_crit() << std::endl;
            //            std::cout << "Stamina: " << gun_data->get_crit_stamina() << std::endl;
            //            std::cout << "Slow: " << gun_data->get_stun() << std::endl;

            //            std::cout << std::endl;

            //            std::cout << "===== Other =====" << std::endl;
            //            std::cout << "Max Ammo: " << gun_data->get_capacity() << std::endl;
            //            std::cout << "Fire Rate: " << gun_data->get_fire_rate() << std::endl;
            //            std::cout << "Stamina Usage: " << gun_data->get_stamina_usage() << std::endl;

            //            std::cout << "Impact Type: " << gun_data->get_impact_type() << std::endl;
            //            std::cout << "Pattern Type: " << gun_data->get_pattern_type() << std::endl;

            //            std::cout << "Auto Fire: " << std::boolalpha << gun_data->get_auto_fire() << std::endl;
            //            std::cout << "Can Inventory: " << std::boolalpha << gun_data->get_can_inventory() << std::endl;
            //        }
            //    }
            //}
        }
        ImGui::EndChild();

        ImGui::BeginChild("lWindow2", ImVec2(0, 125));
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
            ImGui::BeginGroup();
            {
                if (ImGui::CustomCheckbox("Silent Gun", &Settings::Misc::silentGun)) {
                    Modules::Misc::SetSilentGun(Settings::Misc::silentGun);
                }

                if (ImGui::CustomCheckbox("Infinity Ammo", &Settings::Misc::infinityAmmo)) {
                    Settings::Misc::ammoFreezeValue = util::Max(local_mec->get_hand_state().Value_8, Settings::Misc::ammoFreezeValue);
                }

                if (Settings::Misc::infinityAmmo) {
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 1));
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
                    ImGui::InputInt("Freeze Value", &Settings::Misc::ammoFreezeValue, 1, 1);
                    ImGui::PopStyleVar();
                }

                //auto hand_item = local_mec->get_hand_item();
                //if (hand_item) {
                //    auto use_sound = hand_item->get_use_sound();
                //    float use_sound_volume = use_sound->get_volume();
                //    if (ImGui::CustomSliderFloat("Use Sound", &use_sound_volume, 0.0f, 15.0f, "%.2f")) {
                //        use_sound->set_volume(use_sound_volume);
                //    }

                //    float use_sound_pitch = use_sound->get_pitch();
                //    if (ImGui::CustomSliderFloat("Use Pitch", &use_sound_pitch, 0.0f, 15.0f, "%.2f")) {
                //        use_sound->set_pitch(use_sound_pitch);
                //    }

                //    auto hit_drop_sound = hand_item->get_hit_drop_sound();
                //    float hit_drop_sound_volume = hit_drop_sound->get_volume();
                //    if (ImGui::CustomSliderFloat("Hit Drop Sound", &hit_drop_sound_volume, 0.0f, 15.0f, "%.2f")) {
                //        hit_drop_sound->set_volume(hit_drop_sound_volume);
                //    }

                //    auto sneak_drop_sound = hand_item->get_sneak_drop_sound();
                //    float sneak_drop_sound_volume = sneak_drop_sound->get_volume();
                //    if (ImGui::CustomSliderFloat("Sneak Drop Sound", &sneak_drop_sound_volume, 0.0f, 15.0f, " % .2f")) {
                //        sneak_drop_sound->set_volume(sneak_drop_sound_volume);
                //    }

                //    double hit_wall_pitch = hand_item->get_hit_wall_pitch();
                //    if (ImGui::CustomSliderDouble("Hit Wall Pitch", &hit_wall_pitch, 0.0f, 15.0f, "%.2f")) {
                //        hand_item->set_hit_wall_pitch(hit_wall_pitch);
                //    }
                //}
            }
            ImGui::EndGroup();
        }
        ImGui::EndChild();

        ImGui::PopStyleColor();
    }
    ImGui::EndChild();
    ImGui::SameLine();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15.0f);
    ImGui::BeginChild("RightColumn", ImVec2(340, 0));
    {
        //ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        //ImGui::PushStyleColor(ImGuiCol_ChildBg, Colors::WindowNameColor.Value);

        //ImGui::BeginChild("rName1", ImVec2(0, 40));
        //{
        //    ImGui::PushStyleColor(ImGuiCol_Text, Colors::TextDisabled.Value);
        //    ImGui::PushFont(Fonts::RobotoRegular24);

        //    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
        //    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
        //    ImGui::Text("Inventory");

        //    ImGui::PopFont();
        //}
        //ImGui::EndChild();

        //ImGui::PopStyleColor(2);
        //ImGui::PopStyleVar();

        ImGui::PushStyleColor(ImGuiCol_ChildBg, Colors::Window.Value);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));

        ImGui::BeginChild("rWindow1", ImVec2(0, 610));
        {
            auto bag_item = local_mec->get_bag_item();
            auto hand_item = local_mec->get_hand_item();

            if (hand_item && !bag_item) inventory_slot = InventorySlot::Hand;
            else if (!hand_item && bag_item) inventory_slot = InventorySlot::Bag;

            std::string bag_item_name = util::GetValidName(bag_item);
            std::string hand_item_name = util::GetValidName(hand_item);

            float width = (ImGui::GetContentRegionAvail().x);
            float half_width = width / 2;
            ImGui::PushFont(Fonts::InterMedium20);
            if (ImGui::TabButton2("Hand Item", ImVec2(half_width, 40), inventory_slot == InventorySlot::Hand) && hand_item) inventory_slot = InventorySlot::Hand;
            ImGui::SameLine(0.0f, 0.0f);
            if (ImGui::TabButton2("Bag Item", ImVec2(half_width, 40), inventory_slot == InventorySlot::Bag) && bag_item) inventory_slot = InventorySlot::Bag;
            ImGui::PopFont();

            ImGui::BeginGroup();
            {
                ImGui::Spacing();

                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
                ImGui::BeginGroup();
                {
                    width = (ImGui::GetContentRegionAvail().x - 10);
                    half_width = width / 2;

                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 1));

                    if (inventory_slot == InventorySlot::Hand) {
                        if (hand_item) {
                            std::string display_name = hand_item_name;

                            ImGui::Text("Item Name: %s", display_name.c_str());

                            auto hand_state = local_mec->get_hand_state();
                            if (hand_item_name == "GAZ BOTTLE") {
                                const char* colors[] = { "Yellow", "Red", "Blue", "Yellow (4)", "Black" };
                                int current_value = hand_state.Value_8;

                                if (current_value < 0 || current_value > 3) current_value = 4;

                                ImGui::SetNextItemWidth(width);
                                if (ImGui::Combo("##Color", &current_value, colors, IM_ARRAYSIZE(colors))) {
                                    if (current_value < 0 || current_value > 3) hand_state.Value_8 = 4;
                                    else hand_state.Value_8 = current_value;

                                    local_mec->set_hand_state(hand_state);
                                }
                            }
                            else if (hand_item_name == "PACKAGE") {
                                const char* package_types[] = { "Camera", "Computers", "Botanic", "Restaurant", "Medical", "Tutorial" , "Machine" };
                                int package_value = hand_state.Value_8 - 1;

                                ImGui::SetNextItemWidth(width);
                                if (ImGui::Combo("##PackageTypes", &package_value, package_types, IM_ARRAYSIZE(package_types))) {
                                    hand_state.Value_8 = package_value + 1;
                                    local_mec->set_hand_state(hand_state);
                                }
                            }
                            else if (hand_item_name == "FISH") {
                                const char* fish_types[] = { "Salmon", "Tuna", "Cod", "Shrimp" };
                                int fish_value = hand_state.Value_8 - 1;

                                ImGui::SetNextItemWidth(width);
                                if (ImGui::Combo("##FishType", &fish_value, fish_types, IM_ARRAYSIZE(fish_types))) {
                                    hand_state.Value_8 = fish_value + 1;
                                    local_mec->set_hand_state(hand_state);
                                }
                            }
                            else if (hand_item_name == "RICE") {
                                const char* rice_types[] = { "White", "Brown", "Black" };
                                int rice_value = hand_state.Value_8 - 1;

                                ImGui::SetNextItemWidth(width);
                                if (ImGui::Combo("##RiceType", &rice_value, rice_types, IM_ARRAYSIZE(rice_types))) {
                                    hand_state.Value_8 = rice_value + 1;
                                    local_mec->set_hand_state(hand_state);
                                }
                            }
                            else if (hand_item_name == "PACKAGE") {
                                const char* package_types[] = { "Security", "Computers", "Botanic", "Restaurant", "Medical", "Tutorial" , "Machine" };
                                int package_value = hand_state.Value_8 - 1;

                                ImGui::SetNextItemWidth(width);
                                if (ImGui::Combo("##PackageTypes", &package_value, package_types, IM_ARRAYSIZE(package_types))) {
                                    hand_state.Value_8 = package_value + 1;
                                    local_mec->set_hand_state(hand_state);
                                }
                            }
                            else if (hand_item_name == "RICE") {
                                const char* rice_types[] = { "White", "Brown", "Black" };
                                int rice_value = hand_state.Value_8 - 1;

                                ImGui::SetNextItemWidth(width);
                                if (ImGui::Combo("##RiceType", &rice_value, rice_types, IM_ARRAYSIZE(rice_types))) {
                                    hand_state.Value_8 = rice_value + 1;
                                    local_mec->set_hand_state(hand_state);
                                }
                            }
                            else if (hand_item_name == "CASSETTE") {
                                const char* songs[] = { "KHARMA", "Who Am I", "Burning Wish", "Cake", "Puzzle", "Sun", "Worship", "Royalty (Instrumental)", "GRAVE" };
                                int song = hand_state.Value_8;

                                ImGui::SetNextItemWidth(width);
                                if (ImGui::Combo("##Songs", &song, songs, IM_ARRAYSIZE(songs))) {
                                    hand_state.Value_8 = song;
                                    local_mec->set_hand_state(hand_state);
                                }
                            }
                            else if (hand_item_name == "CONTAINER") {
                                const char* colors[] = { "Empty", "Green", "Yellow", "Blue", "White", "Red", "White Rice", "Brown Rice", "Black Rice" };
                                int selected_index;

                                if (hand_state.Value_8 < 6) selected_index = hand_state.Value_8;
                                else selected_index = 6 + (hand_state.Time_15 - 1);

                                ImGui::SetNextItemWidth(width);
                                if (ImGui::Combo("##Color", &selected_index, colors, IM_ARRAYSIZE(colors))) {
                                    if (selected_index == 5) {
                                        hand_state.Value_8 = 5;
                                        hand_state.Time_15 = 1;
                                    }
                                    else if (selected_index >= 6) {
                                        hand_state.Value_8 = 6;
                                        hand_state.Time_15 = selected_index - 5;
                                    }
                                    else {
                                        hand_state.Value_8 = selected_index;
                                        hand_state.Time_15 = 0;
                                    }

                                    local_mec->set_hand_state(hand_state);
                                }
                            }
                            else if (hand_item_name == "SAMPLE") {
                                const char* sample_value_colors[] = { "Empty", "Green", "Yellow", "Blue", "White", "Red" };
                                const char* sample_time_colors[] = { "Empty", "Red" };

                                int value_color = hand_state.Value_8;

                                int time_color;
                                if (value_color == 5) time_color = hand_state.Time_15 - 1;
                                else time_color = hand_state.Time_15;

                                static int prev_value_color = value_color;

                                ImGui::SetNextItemWidth(half_width - 10);
                                if (ImGui::Combo("##SampleValueColor", &value_color, sample_value_colors, IM_ARRAYSIZE(sample_value_colors))) {
                                    hand_state.Value_8 = value_color;

                                    if (value_color == 5) hand_state.Time_15 = 1;
                                    else if (prev_value_color == 5 && hand_state.Time_15 == 1)hand_state.Time_15 = 0;

                                    prev_value_color = value_color;

                                    local_mec->set_hand_state(hand_state);
                                }

                                ImGui::SameLine();

                                ImGui::SetNextItemWidth(half_width - 10);
                                if (ImGui::Combo("##SampleTimeColor", &time_color, sample_time_colors, IM_ARRAYSIZE(sample_time_colors))) {
                                    hand_state.Time_15 = time_color;
                                    local_mec->set_hand_state(hand_state);
                                }
                            }
                            else if (hand_item_name == "FUSE") {
                                const char* fuse_colors[] = { "White", "Red", "Yellow", "Blue", "Black" };
                                int time_color = hand_state.Time_15;
                                int value_color = hand_state.Value_8;

                                if (value_color < 0 || value_color > 3) value_color = 4;
                                if (time_color < 0 || time_color > 3) time_color = 4;

                                ImGui::SetNextItemWidth(half_width - 10);
                                if (ImGui::Combo("##ValueColor", &value_color, fuse_colors, IM_ARRAYSIZE(fuse_colors))) {
                                    hand_state.Value_8 = value_color;
                                    local_mec->set_hand_state(hand_state);
                                }

                                ImGui::SameLine();
                                ImGui::SetNextItemWidth(half_width - 10);
                                if (ImGui::Combo("##TimeColor", &time_color, fuse_colors, IM_ARRAYSIZE(fuse_colors))) {
                                    hand_state.Time_15 = time_color;
                                    local_mec->set_hand_state(hand_state);
                                }
                            }
                            else if (hand_item_name == "PIZZUSHI") {
                                const char* rice_options[] = { "White Rice", "Brown Rice", "Black Rice" };
                                const char* fish_options[] = { "Salmon", "Tuna", "Cod", "Shrimp" };
                                const char* container_colors[] = { "Green", "Yellow", "Blue", "White", "Red" };

                                int value = hand_state.Value_8;
                                int rice_value = value / 100;
                                int fish_value = (value / 10) % 10;
                                int container_value = value % 10;

                                int rice_index = rice_value - 1;
                                int fish_index = fish_value - 1;
                                int container_index = container_value - 1;

                                ImGui::SetNextItemWidth(width);
                                if (ImGui::Combo("##Rice", &rice_index, rice_options, IM_ARRAYSIZE(rice_options))) {
                                    rice_value = rice_index + 1;
                                }
                                ImGui::SetNextItemWidth(width);
                                if (ImGui::Combo("##Fish", &fish_index, fish_options, IM_ARRAYSIZE(fish_options))) {
                                    fish_value = fish_index + 1;
                                }
                                ImGui::SetNextItemWidth(width);
                                if (ImGui::Combo("##Container Color", &container_index, container_colors, IM_ARRAYSIZE(container_colors))) {
                                    container_value = container_index + 1;
                                }

                                hand_state.Value_8 = (rice_value * 100) + (fish_value * 10) + container_value;
                                local_mec->set_hand_state(hand_state);
                            } // Pizzushi

                        }
                    }
                    else if (inventory_slot == InventorySlot::Bag) {
                        if (bag_item) {
                            std::string display_name = bag_item_name;

                            ImGui::Text("Item Name: %s", display_name.c_str());

                            auto bag_state = local_mec->get_bag_state();
                            if (bag_item_name == "GAZ BOTTLE") {
                                const char* colors[] = { "Yellow", "Red", "Blue", "Yellow (4)", "Black" };
                                int current_value = bag_state.Value_8;

                                if (current_value < 0 || current_value > 3) current_value = 4;

                                ImGui::SetNextItemWidth(width);
                                if (ImGui::Combo("##Color", &current_value, colors, IM_ARRAYSIZE(colors))) {
                                    if (current_value < 0 || current_value > 3) bag_state.Value_8 = 4;
                                    else bag_state.Value_8 = current_value;

                                    local_mec->set_bag_state(bag_state);
                                }
                            }
                            else if (bag_item_name == "PACKAGE") {
                                const char* package_types[] = { "Camera", "Computers", "Botanic", "Restaurant", "Medical", "Tutorial" , "Machine" };
                                int package_value = bag_state.Value_8 - 1;

                                ImGui::SetNextItemWidth(width);
                                if (ImGui::Combo("##PackageTypes", &package_value, package_types, IM_ARRAYSIZE(package_types))) {
                                    bag_state.Value_8 = package_value + 1;
                                    local_mec->set_bag_state(bag_state);
                                }
                            }
                            else if (bag_item_name == "FISH") {
                                const char* fish_types[] = { "Salmon", "Tuna", "Cod", "Shrimp" };
                                int fish_value = bag_state.Value_8 - 1;

                                ImGui::SetNextItemWidth(width);
                                if (ImGui::Combo("##FishType", &fish_value, fish_types, IM_ARRAYSIZE(fish_types))) {
                                    bag_state.Value_8 = fish_value + 1;
                                    local_mec->set_bag_state(bag_state);
                                }
                            }
                            else if (bag_item_name == "RICE") {
                                const char* rice_types[] = { "White", "Brown", "Black" };
                                int rice_value = bag_state.Value_8 - 1;

                                ImGui::SetNextItemWidth(width);
                                if (ImGui::Combo("##RiceType", &rice_value, rice_types, IM_ARRAYSIZE(rice_types))) {
                                    bag_state.Value_8 = rice_value + 1;
                                    local_mec->set_bag_state(bag_state);
                                }
                            }
                            else if (bag_item_name == "CASSETTE") {
                                const char* songs[] = { "KHARMA", "Who Am I", "Burning Wish", "Cake", "Puzzle", "Sun", "Worship", "Royalty (Instrumental)", "GRAVE" };
                                int song = bag_state.Value_8;

                                ImGui::SetNextItemWidth(width);
                                if (ImGui::Combo("##Songs", &song, songs, IM_ARRAYSIZE(songs))) {
                                    bag_state.Value_8 = song;
                                    local_mec->set_bag_state(bag_state);
                                }
                            }
                            else if (bag_item_name == "CONTAINER") {
                                const char* colors[] = { "Empty", "Green", "Yellow", "Blue", "White", "Red", "White Rice", "Brown Rice", "Black Rice" };
                                int selected_index;

                                if (bag_state.Value_8 < 6) selected_index = bag_state.Value_8;
                                else  selected_index = 6 + (bag_state.Time_15 - 1);

                                ImGui::SetNextItemWidth(width);
                                if (ImGui::Combo("##Color", &selected_index, colors, IM_ARRAYSIZE(colors))) {
                                    if (selected_index == 5) {
                                        bag_state.Value_8 = 5;
                                        bag_state.Time_15 = 1;
                                    }
                                    else if (selected_index >= 6) {
                                        bag_state.Value_8 = 6;
                                        bag_state.Time_15 = selected_index - 5;
                                    }
                                    else {
                                        bag_state.Value_8 = selected_index;
                                        bag_state.Time_15 = 0;
                                    }

                                    local_mec->set_bag_state(bag_state);
                                }
                            }
                            else if (bag_item_name == "SAMPLE") {
                                const char* sample_value_colors[] = { "Empty", "Green", "Yellow", "Blue", "White", "Red" };
                                const char* sample_time_colors[] = { "Empty", "Red" };

                                int value_color = bag_state.Value_8;

                                int time_color;
                                if (value_color == 5) time_color = bag_state.Time_15 - 1;
                                else time_color = bag_state.Time_15;

                                static int prev_value_color = value_color;

                                ImGui::SetNextItemWidth(half_width - 10);
                                if (ImGui::Combo("##SampleValueColor", &value_color, sample_value_colors, IM_ARRAYSIZE(sample_value_colors))) {
                                    bag_state.Value_8 = value_color;

                                    if (value_color == 5) bag_state.Time_15 = 1;
                                    else if (prev_value_color == 5 && bag_state.Time_15 == 1) bag_state.Time_15 = 0;

                                    prev_value_color = value_color;

                                    local_mec->set_bag_state(bag_state);
                                }

                                ImGui::SameLine();

                                ImGui::SetNextItemWidth(half_width - 10);
                                if (ImGui::Combo("##SampleTimeColor", &time_color, sample_time_colors, IM_ARRAYSIZE(sample_time_colors))) {
                                    bag_state.Time_15 = time_color;
                                    local_mec->set_bag_state(bag_state);
                                }
                            }
                            else if (bag_item_name == "FUSE") {
                                const char* fuse_colors[] = { "White", "Red", "Yellow", "Blue", "Black" };
                                int time_color = bag_state.Time_15;
                                int value_color = bag_state.Value_8;

                                if (value_color < 0 || value_color > 3) value_color = 4;
                                if (time_color < 0 || time_color > 3) time_color = 4;

                                ImGui::SetNextItemWidth(half_width - 10);
                                if (ImGui::Combo("##ValueColor", &value_color, fuse_colors, IM_ARRAYSIZE(fuse_colors))) {
                                    bag_state.Value_8 = value_color;
                                    local_mec->set_bag_state(bag_state);
                                }

                                ImGui::SameLine();
                                ImGui::SetNextItemWidth(half_width - 10);
                                if (ImGui::Combo("##TimeColor", &time_color, fuse_colors, IM_ARRAYSIZE(fuse_colors))) {
                                    bag_state.Time_15 = time_color;
                                    local_mec->set_bag_state(bag_state);
                                }
                            }
                            else if (bag_item_name == "PIZZUSHI") { // Pizzushi
                                const char* rice_options[] = { "White Rice", "Brown Rice", "Black Rice" };
                                const char* fish_options[] = { "Salmon", "Tuna", "Cod", "Shrimp" };
                                const char* container_colors[] = { "Green", "Yellow", "Blue", "White", "Red" };

                                int value = bag_state.Value_8;
                                int rice_value = value / 100;
                                int fish_value = (value / 10) % 10;
                                int container_value = value % 10;

                                int rice_index = rice_value - 1;
                                int fish_index = fish_value - 1;
                                int container_index = container_value - 1;

                                ImGui::SetNextItemWidth(width);
                                if (ImGui::Combo("##Rice", &rice_index, rice_options, IM_ARRAYSIZE(rice_options))) {
                                    rice_value = rice_index + 1;
                                }
                                ImGui::SetNextItemWidth(width);
                                if (ImGui::Combo("##Fish", &fish_index, fish_options, IM_ARRAYSIZE(fish_options))) {
                                    fish_value = fish_index + 1;
                                }
                                ImGui::SetNextItemWidth(width);
                                if (ImGui::Combo("##Container Color", &container_index, container_colors, IM_ARRAYSIZE(container_colors))) {
                                    container_value = container_index + 1;
                                }
                                bag_state.Value_8 = (rice_value * 100) + (fish_value * 10) + container_value;
                                local_mec->set_bag_state(bag_state);
                            }
                        }
                    }

                    ImGui::PopStyleVar();
                }
                ImGui::EndGroup();

                ImGui::Spacing();

                if (inventory_slot == InventorySlot::Hand) {
                    if (hand_item) {
                        auto hand_state = local_mec->get_hand_state();
                        auto gun_data = (u_data_gun*)hand_item;

                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
                        ImGui::BeginGroup();
                        {
                            if (ImGui::InputInt("Value8", reinterpret_cast<int*>(&hand_state.Value_8)))
                                local_mec->set_hand_state(hand_state);

                            if (ImGui::InputInt("Time15", reinterpret_cast<int*>(&hand_state.Time_15)))
                                local_mec->set_hand_state(hand_state);

                            /*
                             if (hand_item_name != "C4" && hand_item_name != "DETONATOR" && hand_item_name != "SCREW DRIVER" && hand_item_name != "KNIFE") {
                                if (util::get_name_from_fname(hand_item->class_private()->fname_index()).find("Data_Gun_C") != std::string::npos || hand_item_name == "PACKAGE") {
                                    if (ImGui::InputInt("Ammo", reinterpret_cast<int*>(&hand_state.Value_8)))
                                        local_mec->set_hand_state(hand_state);
                                }
                                else {
                                    if (ImGui::InputInt("State", reinterpret_cast<int*>(&hand_state.Value_8)))
                                        local_mec->set_hand_state(hand_state);
                                }
                            }

                            if (hand_item_name == "FUSE" || hand_item_name == "SAMPLE" || hand_item_name == "CONTAINER" || hand_item_name == "PACKAGE" || hand_item_name == "RICE") {
                                if (ImGui::InputInt("Type", reinterpret_cast<int*>(&hand_state.Time_15))) {
                                    local_mec->set_hand_state(hand_state);
                                }
                            }
                            */
                        }
                        ImGui::EndGroup();

                        if (util::get_name_from_fname(hand_item->class_private()->fname_index()).find("Data_Gun_C") != std::string::npos) {

                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
                            ImGui::BeginGroup();
                            {
                                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 1));
                                ImGui::Text("Body Damage");

                                int body_damage = gun_data->get_damage();
                                if (ImGui::InputInt("Health##body", &body_damage)) {
                                    gun_data->set_damage(body_damage);
                                }

                                int body_stamina = gun_data->get_stamina_damage();
                                if (ImGui::InputInt("Stamina##body", &body_stamina)) {
                                    gun_data->set_stamina_damage(body_stamina);
                                }

                                double body_slow = gun_data->get_stun();
                                auto display_body_slow = 100.0 * body_slow;
                                if (ImGui::InputDouble("Knockback##body", &display_body_slow, 1.0, 1.0, "%.0lf")) {
                                    body_slow = display_body_slow / 100.0;
                                    gun_data->set_stun(body_slow);
                                }

                                ImGui::Spacing();
                                ImGui::Spacing();

                                ImGui::Text("Head Damage");

                                int head_damage = gun_data->get_crit();
                                if (ImGui::InputInt("Health##head", &head_damage)) {
                                    gun_data->set_crit(head_damage);
                                }

                                int head_stamina = gun_data->get_crit_stamina();
                                if (ImGui::InputInt("Stamina##head", &head_stamina)) {
                                    gun_data->set_crit_stamina(head_stamina);
                                }

                                if (ImGui::InputDouble("Knockback##Head", &display_body_slow, 1.0, 1.0, "%.0lf")) {
                                    body_slow = display_body_slow / 100.0;
                                    gun_data->set_stun(body_slow);
                                }

                                ImGui::Spacing();
                                ImGui::Spacing();

                                auto capacity = gun_data->get_capacity();
                                if (ImGui::InputInt("Max Ammo", &capacity)) {
                                    gun_data->set_capacity(capacity);
                                }

                                auto fire_rate = gun_data->get_fire_rate();
                                double display_value = 60.0 / fire_rate;
                                if (ImGui::InputDouble("Fire Rate", &display_value, 1.0, 1.0, "%.0lf")) {
                                    fire_rate = 60.0 / display_value;
                                    gun_data->set_fire_rate(fire_rate);
                                }

                                ImGui::Spacing();

                                auto stamina_cost = gun_data->get_stamina_usage();
                                if (ImGui::InputInt("Stamina Cost", &stamina_cost)) {
                                    gun_data->set_stamina_usage(stamina_cost);
                                }

                                int impact_type = gun_data->get_impact_type();
                                if (ImGui::InputInt("Impact Type", &impact_type)) {
                                    gun_data->set_impact_type(impact_type);
                                }

                                int pattern_type = gun_data->get_pattern_type();
                                if (ImGui::InputInt("Pattern Type", &pattern_type)) {
                                    gun_data->set_pattern_type(pattern_type);
                                }

                                int logic = hand_item->get_logic();
                                if (ImGui::InputInt("##Logic", &logic, 1, 1)) {
                                    logic = std::clamp(logic, 0, 13);      
                                    hand_item->set_logic(logic);
                                }

                                ImGui::SameLine();
                                ImGui::Text("%s", E_Logic_Names[logic]);

                                ImGui::PopStyleVar();
                            }
                            ImGui::EndGroup();

                            ImGui::Spacing();
                            ImGui::Separator();

                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
                            if (ImGui::CustomButton("Reset stats##weaponhand", ImVec2(half_width - 5, 25))) {
                                auto item_stats = GetItemProperties(hand_item_name);
                                gun_data->set_damage(item_stats.body_damage_health);
                                gun_data->set_stamina_damage(item_stats.body_damage_stamina);
                                gun_data->set_stun(item_stats.body_damage_slow);

                                gun_data->set_crit(item_stats.head_damage_health);
                                gun_data->set_crit_stamina(item_stats.head_damage_stamina);

                                gun_data->set_capacity(item_stats.max_ammo);
                                gun_data->set_fire_rate(item_stats.fire_rate);
                                gun_data->set_stamina_usage(item_stats.stamina_cost);

                                gun_data->set_impact_type(item_stats.impact_type);
                                gun_data->set_pattern_type(item_stats.pattern_type);

                                gun_data->set_auto_fire(item_stats.auto_fire);
                                gun_data->set_can_inventory(item_stats.can_inventory);
                            }

                            ImGui::SameLine();

                            if (ImGui::CustomButton("Clear Hand item", ImVec2(half_width - 5, 25))) local_mec->set_hand_item(nullptr);

                            auto auto_fire = gun_data->get_auto_fire();
                            if (ImGui::CustomCheckbox("Auto Fire", &auto_fire)) {
                                gun_data->set_auto_fire(auto_fire);
                            }
                        }
                        else if (util::get_name_from_fname(hand_item->class_private()->fname_index()).find("Data_Melee_C") != std::string::npos) {
                            auto melee_data = (u_data_melee*)hand_item;
                            auto melee_type = melee_data->get_melee_type();

                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
                            ImGui::BeginGroup();
                            {
                                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 1));
                                ImGui::Text("Frontal Damage");

                                int frontal_damage = melee_type->get_heal();
                                if (ImGui::InputInt("Health##frontal", &frontal_damage)) {
                                    melee_type->set_heal(frontal_damage);
                                }

                                int frontal_stamina = melee_type->get_stamina();
                                if (ImGui::InputInt("Stamina##frontal", &frontal_stamina)) {
                                    melee_type->set_stamina(frontal_stamina);
                                }

                                double frontal_slow = melee_type->get_stun();
                                // auto frontal_stat_type = (frontal_slow < 0) ? "Frontal Knockback" : "Frontal Slow";
                                auto display_frontal_slow = 100.0 * frontal_slow;
                                if (ImGui::InputDouble("Knockback##frontal", &display_frontal_slow, 1.0, 1.0, "%.0lf")) {
                                    frontal_slow = display_frontal_slow / 100.0;
                                    melee_type->set_stun(frontal_slow);
                                }

                                ImGui::Spacing();
                                ImGui::Spacing();

                                ImGui::Text("Back Damage");

                                int back_damage = melee_type->get_crit_heal();
                                if (ImGui::InputInt("Health##back", &back_damage)) {
                                    melee_type->set_crit_heal(back_damage);
                                }

                                int back_stamina = melee_type->get_crit_stamina();
                                if (ImGui::InputInt("Stamina##back", &back_stamina)) {
                                    melee_type->set_crit_stamina(back_stamina);
                                }

                                double back_slow = melee_type->get_crit_stun();
                                // auto back_stat_type = (frontal_slow < 0) ? "Back Knockback" : "Back Slow";
                                auto display_back_slow = 100.0 * back_slow;
                                if (ImGui::InputDouble("Knockback##back", &display_back_slow, 1.0, 1.0, "%.0lf")) {
                                    back_slow = display_back_slow / 100.0;
                                    melee_type->set_crit_stun(back_slow);
                                }

                                ImGui::Spacing();
                                ImGui::Spacing();

                                int range = melee_type->get_range();
                                if (ImGui::InputInt("Attack Range", &range)) {
                                    melee_type->set_range(range);
                                }

                                double attack_speed = melee_type->get_recover_time();
                                if (ImGui::InputDouble("Attack Speed", &attack_speed, 1.0, 1.0, "%.2lf")) {
                                    melee_type->set_recover_time(attack_speed);
                                }

                                ImGui::Spacing();

                                auto stamina_cost = melee_type->get_cost();
                                if (ImGui::InputInt("Stamina Cost", &stamina_cost)) {
                                    melee_type->set_cost(stamina_cost);
                                }

                                double cast_time = melee_type->get_cast_time();
                                if (ImGui::InputDouble("Cast Time", &cast_time, 1.0, 1.0, "%.2lf")) {
                                    melee_type->set_cast_time(cast_time);
                                }

                                int logic = hand_item->get_logic();
                                if (ImGui::InputInt("##Logic", &logic, 1, 1)) {
                                    logic = std::clamp(logic, 0, 13);
                                    hand_item->set_logic(logic);
                                }

                                ImGui::SameLine();
                                ImGui::Text("%s", E_Logic_Names[logic]);

                                ImGui::PopStyleVar();
                            }
                            ImGui::EndGroup();

                            ImGui::Spacing();
                            ImGui::Separator();

                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
                            if (ImGui::CustomButton("Reset stats##meleehand", ImVec2(half_width - 5, 25))) {
                                auto item_stats = GetItemProperties(hand_item_name);
                                melee_type->set_heal(item_stats.frontal_damage_health);
                                melee_type->set_stamina(item_stats.frontal_damage_stamina);
                                melee_type->set_stun(item_stats.frontal_damage_slow);

                                melee_type->set_crit_heal(item_stats.back_damage_health);
                                melee_type->set_crit_stamina(item_stats.back_damage_stamina);
                                melee_type->set_crit_stun(item_stats.back_damage_slow);

                                melee_type->set_range(item_stats.attack_range);
                                melee_type->set_recover_time(item_stats.attack_speed);
                                melee_type->set_cost(item_stats.stamina_cost);
                                melee_type->set_cast_time(item_stats.cast_time);

                                gun_data->set_can_inventory(item_stats.can_inventory);
                            }

                            ImGui::SameLine();

                            if (ImGui::CustomButton("Clear Hand Item", ImVec2(half_width - 5, 25))) local_mec->set_hand_item(nullptr);
                        }

                        auto can_inventory = gun_data->get_can_inventory();
                        if (ImGui::CustomCheckbox("Can inventory", &can_inventory)) {
                            gun_data->set_can_inventory(can_inventory);
                        }

                        if(ImGui::CustomCheckbox("Freeze Item##Hand", &Settings::Misc::freezeHandItem)) {
                            if (Settings::Misc::freezeHandItem) {
                                Settings::Misc::saved_hand_item = hand_item;
                                Settings::Misc::saved_hand_state = local_mec->get_hand_state();
                            }
                            else {
                                Settings::Misc::saved_hand_item = nullptr;
                            }
                        }
                    }
                }
                else if (inventory_slot == InventorySlot::Bag) {
                    if (bag_item) {
                        auto bag_state = local_mec->get_bag_state();
                        auto gun_data = (u_data_gun*)bag_item;

                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
                        ImGui::BeginGroup();
                        {
                            if (ImGui::InputInt("Value8", reinterpret_cast<int*>(&bag_state.Value_8)))
                                local_mec->set_bag_state(bag_state);

                            if (ImGui::InputInt("Time15", reinterpret_cast<int*>(&bag_state.Time_15)))
                                local_mec->set_bag_state(bag_state);

                            /*
                             if (hand_item_name != "C4" && hand_item_name != "DETONATOR" && hand_item_name != "SCREW DRIVER" && hand_item_name != "KNIFE") {
                                if (util::get_name_from_fname(hand_item->class_private()->fname_index()).find("Data_Gun_C") != std::string::npos || hand_item_name == "PACKAGE") {
                                    if (ImGui::InputInt("Ammo", reinterpret_cast<int*>(&hand_state.Value_8)))
                                        local_mec->set_hand_state(hand_state);
                                }
                                else {
                                    if (ImGui::InputInt("State", reinterpret_cast<int*>(&hand_state.Value_8)))
                                        local_mec->set_hand_state(hand_state);
                                }
                            }

                            if (hand_item_name == "FUSE" || hand_item_name == "SAMPLE" || hand_item_name == "CONTAINER" || hand_item_name == "PACKAGE" || hand_item_name == "RICE") {
                                if (ImGui::InputInt("Type", reinterpret_cast<int*>(&hand_state.Time_15))) {
                                    local_mec->set_hand_state(hand_state);
                                }
                            }
                            */
                        }
                        ImGui::EndGroup();

                        if (util::get_name_from_fname(bag_item->class_private()->fname_index()).find("Data_Gun_C") != std::string::npos) {

                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
                            ImGui::BeginGroup();
                            {
                                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 1));

                                ImGui::Text("Body Damage");

                                int body_damage = gun_data->get_damage();
                                if (ImGui::InputInt("Health##body", &body_damage)) {
                                    gun_data->set_damage(body_damage);
                                }

                                int body_stamina = gun_data->get_stamina_damage();
                                if (ImGui::InputInt("Stamina##body", &body_stamina)) {
                                    gun_data->set_stamina_damage(body_stamina);
                                }

                                double body_slow = gun_data->get_stun();
                                auto display_body_slow = 100.0 * body_slow;
                                if (ImGui::InputDouble("Knockback##body", &display_body_slow, 1.0, 1.0, "%.0lf")) {
                                    body_slow = display_body_slow / 100.0;
                                    gun_data->set_stun(body_slow);
                                }

                                ImGui::Spacing();
                                ImGui::Spacing();

                                ImGui::Text("Head Damage");

                                int head_damage = gun_data->get_crit();
                                if (ImGui::InputInt("Health##head", &head_damage)) {
                                    gun_data->set_crit(head_damage);
                                }

                                int head_stamina = gun_data->get_crit_stamina();
                                if (ImGui::InputInt("Stamina##head", &head_stamina)) {
                                    gun_data->set_crit_stamina(head_stamina);
                                }

                                if (ImGui::InputDouble("Knockback##Head", &display_body_slow, 1.0, 1.0, "%.0lf")) {
                                    body_slow = display_body_slow / 100.0;
                                    gun_data->set_stun(body_slow);
                                }

                                ImGui::Spacing();
                                ImGui::Spacing();

                                auto capacity = gun_data->get_capacity();
                                if (ImGui::InputInt("Max Ammo", &capacity)) {
                                    gun_data->set_capacity(capacity);
                                }

                                auto fire_rate = gun_data->get_fire_rate();
                                double display_fire_rate = 60.0 / fire_rate;
                                if (ImGui::InputDouble("Fire Rate", &display_fire_rate, 1.0, 1.0, "%.0lf")) {
                                    fire_rate = 60.0 / display_fire_rate;
                                    gun_data->set_fire_rate(fire_rate);
                                }

                                ImGui::Spacing();

                                auto stamina_cost = gun_data->get_stamina_usage();
                                if (ImGui::InputInt("Stamina Cost", &stamina_cost)) {
                                    gun_data->set_stamina_usage(stamina_cost);
                                }

                                int impact_type = gun_data->get_impact_type();
                                if (ImGui::InputInt("Impact Type", &impact_type)) {
                                    gun_data->set_impact_type(impact_type);
                                }

                                int pattern_type = gun_data->get_pattern_type();
                                if (ImGui::InputInt("Pattern Type", &pattern_type)) {
                                    gun_data->set_pattern_type(pattern_type);
                                }

                                int logic = hand_item->get_logic();
                                if (ImGui::InputInt("##Logic", &logic, 1, 1)) {
                                    logic = std::clamp(logic, 0, 13);
                                    hand_item->set_logic(logic);
                                }

                                ImGui::SameLine();
                                ImGui::Text("%s", E_Logic_Names[logic]);

                                ImGui::PopStyleVar();
                            }
                            ImGui::EndGroup();

                            ImGui::Spacing();
                            ImGui::Separator();

                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
                            if (ImGui::CustomButton("Reset stats##weaponbag", ImVec2(half_width - 5, 25))) {
                                auto item_stats = GetItemProperties(bag_item_name);
                                gun_data->set_damage(item_stats.body_damage_health);
                                gun_data->set_stamina_damage(item_stats.body_damage_stamina);
                                gun_data->set_stun(item_stats.body_damage_slow);

                                gun_data->set_crit(item_stats.head_damage_health);
                                gun_data->set_crit_stamina(item_stats.head_damage_stamina);

                                gun_data->set_capacity(item_stats.max_ammo);
                                gun_data->set_fire_rate(item_stats.fire_rate);
                                gun_data->set_stamina_usage(item_stats.stamina_cost);

                                gun_data->set_impact_type(item_stats.impact_type);
                                gun_data->set_pattern_type(item_stats.pattern_type);

                                gun_data->set_auto_fire(item_stats.auto_fire);
                                gun_data->set_can_inventory(item_stats.can_inventory);
                            }

                            ImGui::SameLine();

                            if (ImGui::CustomButton("Clear Bag Item", ImVec2(half_width - 5, 25))) local_mec->set_bag_item(nullptr);

                            auto auto_fire = gun_data->get_auto_fire();
                            if (ImGui::CustomCheckbox("Auto Fire", &auto_fire)) {
                                gun_data->set_auto_fire(auto_fire);
                            }
                        }
                        else if (util::get_name_from_fname(bag_item->class_private()->fname_index()).find("Data_Melee_C") != std::string::npos) {
                            auto melee_data = (u_data_melee*)bag_item;
                            auto melee_type = melee_data->get_melee_type();

                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
                            ImGui::BeginGroup();
                            {
                                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 1));

                                ImGui::Text("Frontal Damage");

                                int frontal_damage = melee_type->get_heal();
                                if (ImGui::InputInt("Health##frontal", &frontal_damage)) {
                                    melee_type->set_heal(frontal_damage);
                                }

                                int frontal_stamina = melee_type->get_stamina();
                                if (ImGui::InputInt("Stamina##frontal", &frontal_stamina)) {
                                    melee_type->set_stamina(frontal_stamina);
                                }

                                double frontal_slow = melee_type->get_stun();
                                auto display_frontal_slow = 100.0 * frontal_slow;
                                if (ImGui::InputDouble("Knockback##frontal", &display_frontal_slow, 1.0, 1.0, "%.0lf")) {
                                    frontal_slow = display_frontal_slow / 100.0;
                                    melee_type->set_stun(frontal_slow);
                                }

                                ImGui::Spacing();
                                ImGui::Spacing();

                                ImGui::Text("Back Damage");

                                int back_damage = melee_type->get_crit_heal();
                                if (ImGui::InputInt("Health##back", &back_damage)) {
                                    melee_type->set_crit_heal(back_damage);
                                }

                                int back_stamina = melee_type->get_crit_stamina();
                                if (ImGui::InputInt("Stamina##back", &back_stamina)) {
                                    melee_type->set_crit_stamina(back_stamina);
                                }

                                double back_slow = melee_type->get_crit_stun();
                                auto display_back_slow = 100.0 * back_slow;
                                if (ImGui::InputDouble("Knockback##back", &display_back_slow, 1.0, 1.0, "%.0lf")) {
                                    back_slow = display_back_slow / 100.0;
                                    melee_type->set_crit_stun(back_slow);
                                }

                                ImGui::Spacing();
                                ImGui::Spacing();

                                int range = melee_type->get_range();
                                if (ImGui::InputInt("Attack Range", &range)) {
                                    melee_type->set_range(range);
                                }

                                double attack_speed = melee_type->get_recover_time();
                                if (ImGui::InputDouble("Attack Speed", &attack_speed, 1.0, 1.0, "%.2lf")) {
                                    melee_type->set_recover_time(attack_speed);
                                }

                                ImGui::Spacing();

                                auto stamina_cost = melee_type->get_cost();
                                if (ImGui::InputInt("Stamina Cost", &stamina_cost)) {
                                    melee_type->set_cost(stamina_cost);
                                }

                                double cast_time = melee_type->get_cast_time();
                                if (ImGui::InputDouble("Cast Time", &cast_time, 1.0, 1.0, "%.2lf")) {
                                    melee_type->set_cast_time(cast_time);
                                }

                                int logic = hand_item->get_logic();
                                if (ImGui::InputInt("##Logic", &logic, 1, 1)) {
                                    logic = std::clamp(logic, 0, 13);
                                    hand_item->set_logic(logic);
                                }

                                ImGui::SameLine();
                                ImGui::Text("%s", E_Logic_Names[logic]);

                                ImGui::PopStyleVar();
                            }
                            ImGui::EndGroup();

                            ImGui::Spacing();
                            ImGui::Separator();

                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
                            if (ImGui::CustomButton("Reset stats##meleebag", ImVec2(half_width - 5, 25))) {
                                auto item_stats = GetItemProperties(bag_item_name);
                                melee_type->set_heal(item_stats.frontal_damage_health);
                                melee_type->set_stamina(item_stats.frontal_damage_stamina);
                                melee_type->set_stun(item_stats.frontal_damage_slow);

                                melee_type->set_crit_heal(item_stats.back_damage_health);
                                melee_type->set_crit_stamina(item_stats.back_damage_stamina);
                                melee_type->set_crit_stun(item_stats.back_damage_slow);

                                melee_type->set_range(item_stats.attack_range);
                                melee_type->set_recover_time(item_stats.attack_speed);
                                melee_type->set_cost(item_stats.stamina_cost);
                                melee_type->set_cast_time(item_stats.cast_time);

                                gun_data->set_can_inventory(item_stats.can_inventory);
                            }

                            ImGui::SameLine();

                            if (ImGui::CustomButton("Clear Bag Item", ImVec2(half_width - 5, 25))) local_mec->set_bag_item(nullptr);;
                        }

                        auto can_inventory = gun_data->get_can_inventory();
                        if (ImGui::CustomCheckbox("Can inventory", &can_inventory)) {
                            gun_data->set_can_inventory(can_inventory);
                        }

                        if (ImGui::CustomCheckbox("Freeze Item##Bag", &Settings::Misc::freezeBagItem)) {
                            if (Settings::Misc::freezeBagItem) {
                                Settings::Misc::saved_bag_item = bag_item;
                                Settings::Misc::saved_bag_state = local_mec->get_bag_state();
                            }
                            else {
                                Settings::Misc::saved_bag_item = nullptr;
                            }
                        }
                    }
                }

            }
            ImGui::EndGroup();

        }
        ImGui::EndChild();

        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
}

void Menu::RenderPlayerTab() {
    ImGui::BeginChild("LeftColumn", ImVec2(330, 0));
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, Colors::Window.Value);
        ImGui::BeginChild("lWindow1", ImVec2(0, 610));
        {
            ImGui::PopStyleColor();
            ImGui::SetCursorPos(ImVec2{ ImGui::GetCursorPosX() + 10.0f, ImGui::GetCursorPosY() + 10.0f });
            ImGui::BeginGroup();
            {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 1));

                int health = local_mec->get_health();
                if (ImGui::InputInt("Health", &health)) {
                    local_mec->set_health(health);
                }

                double stamina = local_mec->get_stamina();
                int max_stamina = static_cast<int>(health);
                int display_stamina = max_stamina - static_cast<int>(stamina);

                if (ImGui::InputInt("Stamina", &display_stamina)) {
                    local_mec->set_stamina(static_cast<double>(max_stamina - display_stamina));
                }

                auto mec_data = local_mec->get_player_data();
                double health_regen = mec_data->get_regen_hp_speed();
                if (ImGui::InputDouble("Health Regen", &health_regen, 1.0, 1.0, "%.2lf")) {
                    mec_data->set_regen_hp_speed(health_regen);
                }

                ImGui::Spacing();

                double max_speed = local_mec->get_max_speed();
                if (ImGui::InputDouble("Max Speed", &max_speed, 1.0, 1.0, "%.0f")) {
                    local_mec->set_max_speed(max_speed);
                }

                ImGui::InputDouble("Acceleration", &Settings::Player::acceleration, 1.0, 1.0, "%.0f");
                
                uint8_t step_handling = local_mec->get_step_handling();
                int tmp_handling = step_handling;
                if (ImGui::InputInt("Step Handling", &tmp_handling, 1, 1, ImGuiInputTextFlags_None)) {
                    tmp_handling = std::clamp(tmp_handling, 0, 255);
                    step_handling = static_cast<uint8_t>(tmp_handling);
                    local_mec->set_step_handling(step_handling);
                    Settings::Player::stepHandling = step_handling;
                }

                //int Net_Fish = local_mec->get_net_fish();
                //if (ImGui::InputInt("Net_Fish", &Net_Fish)) {
                //    local_mec->set_net_fish(Net_Fish);
                //}

                ImGui::Spacing();

                auto location = util::ConvertFVectorToVector3(local_mec->get_net_location());

                if (ImGui::InputDouble("X", &location.x, 1.0, 1.0, "%.0lf") && ImGui::IsItemDeactivatedAfterEdit()) {
                    local_mec->teleport(location);
                }

                if (ImGui::InputDouble("Y", &location.y, 1.0, 1.0, "%.0lf") && ImGui::IsItemDeactivatedAfterEdit()) {
                    local_mec->teleport(location);
                }

                if (ImGui::InputDouble("Z", &location.z, 1.0, 1.0, "%.0lf") && ImGui::IsItemDeactivatedAfterEdit()) {
                    local_mec->teleport(location);
                }

                //ImGui::Spacing();
                //ImGui::Spacing();

                //auto camera = local_mec->get_camera();
                //auto camera_location = camera->get_location();

                //if (ImGui::InputDouble("X##Camera", &camera_location.x, 1.0, 1.0, "%.0lf") && ImGui::IsItemDeactivatedAfterEdit()) {
                //    camera->set_location(camera_location);
                //}

                //if (ImGui::InputDouble("Y##Camera", &camera_location.y, 1.0, 1.0, "%.0lf") && ImGui::IsItemDeactivatedAfterEdit()) {
                //    camera->set_location(camera_location);
                //}

                //if (ImGui::InputDouble("Z##Camera", &camera_location.z, 1.0, 1.0, "%.0lf") && ImGui::IsItemDeactivatedAfterEdit()) {
                //    camera->set_location(camera_location);
                //}

                ImGui::PopStyleVar();
            }
            ImGui::EndGroup();

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::CustomSliderFloat("FOV##player", &Settings::Player::fov, 1.0f, 179.9f, "%.2f");

            //double roll = local_mec->get_roll();
            //if (ImGui::InputDouble("Roll", &roll, 1.0, 1.0, "%.0f")) {
            //    local_mec->set_roll(roll);
            //}

            //auto free_cam = local_camera_manager->get_free_cam_distance();
            //if (ImGui::CustomSliderFloat("FreeCam Distance##player", &free_cam, 1.0f, 500.0f, "%.2f")) {
            //    local_camera_manager->set_free_cam_distance(free_cam);
            //}

            //auto update_cam = local_camera_manager->get_server_cam_update();
            //if (ImGui::CustomSliderFloat("Server Cam Update##player", &update_cam, 1.0f, 10000.0f, "%.2f")) {
            //    local_camera_manager->set_server_cam_update(update_cam);
            //}

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            Settings::Player::alive = local_mec->get_alive();
            if (ImGui::CustomCheckbox("Alive", &Settings::Player::alive)) {
                Modules::Misc::SetAlive(Settings::Player::alive);
            }

            if (ImGui::CustomCheckbox("SpeedHack", &Settings::Misc::speedHack)) {
                Settings::Player::stepHandling = local_mec->get_step_handling();
            }

            ImGui::CustomCheckbox("Run Gun", &Settings::Player::runGun);
            ImGui::CustomCheckbox("Infinity Stamina", &Settings::Misc::infinityStamina);
            ImGui::CustomCheckbox("Infinity Fly Stamina", &Settings::Misc::infinityFlyStamina);
            ImGui::CustomCheckbox("God Mode", &Settings::Misc::godMode);
            //ImGui::CustomCheckbox("Speed Hack", &Settings::Misc::speedHack);
            ImGui::CustomCheckbox("No Spread", &Settings::Misc::noSpread);
            ImGui::CustomCheckbox("Rainbow Skin", &Settings::Misc::rainbowSkin);


            auto skin_set = local_mec->get_skin_set();
            int color = skin_set.Color_8;
            std::string color_name;
            switch (color) {
            case 0: color_name = "Black"; break;
            case 1: color_name = "Red"; break;
            case 2: color_name = "Orange"; break;
            case 3: color_name = "Yellow"; break;
            case 4: color_name = "Green"; break;
            case 5: color_name = "Cyan"; break;
            case 6: color_name = "Blue"; break;
            case 7: color_name = "Purple"; break;
            case 8: color_name = "Pink"; break;
            case 9: color_name = "White"; break;
            case 10: color_name = "Black"; break;
            default:
                color_name = "Unknown";
            }

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 1));
            std::string input_label = color_name;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
            if (ImGui::InputInt(input_label.c_str(), &color)) {
                skin_set.Color_8 = color;
                local_mec->set_skin_set(skin_set);

                local_mec->get_skin_save()->set_setting(skin_set);
            }
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
            ImGui::InputInt("Rainbow Delay", &Settings::Misc::rainbowSkinDelay);
            ImGui::PopStyleVar();
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15.0f);
    ImGui::BeginChild("RightColumn", ImVec2(330, 0));
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, Colors::Window.Value);
        ImGui::BeginChild("rWindow1", ImVec2(0, 610));
        {
            ImGui::PopStyleColor();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7.5);
            ImGui::BeginGroup();
            {
                if (!Settings::FreeCam::enabled) {
                    ImGui::CustomCheckbox("Enable FlyHack", &Settings::FlyHack::enabled);
                }
                else {
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                    ImGui::CustomCheckbox("Enable FlyHack", &Settings::FlyHack::enabled);
                    ImGui::PopItemFlag();
                }

                //if (!Settings::FlyHack::enabled) {
                //    ImGui::CustomCheckbox("Enable FreeCam", &Settings::FreeCam::enabled);
                //}
                //else {
                //    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                //    ImGui::CustomCheckbox("Enable FreeCam", &Settings::FreeCam::enabled);
                //    ImGui::PopItemFlag();
                //}
                //ImGui::SameLine(ImGui::GetContentRegionAvail().x - 140);

                //ImHotkey("Enable FlyHack", &Settings::FlyHack::hotkey, ImVec2(60, 20));
                ImGui::Spacing();
                ImGui::Spacing();

                //ImGui::CustomCheckbox("NoPush", &Settings::FlyHack::noPush);
                ImGui::CustomCheckbox("NoFall", &Settings::FlyHack::noFall);
                //ImGui::CustomCheckbox("NoClip", &Settings::FlyHack::noClip);

                ImGui::Spacing();
                ImGui::Spacing();

                ImGui::CustomSliderFloat("Horizontal", &Settings::FlyHack::hSpeed, 0.01f, 500.0f, "%.2f");
                ImGui::CustomSliderFloat("Vertical", &Settings::FlyHack::vSpeed, 0.01f, 500.0f, "%.2f");
                ImGui::CustomCheckbox("Directional", &Settings::FlyHack::directional);

            }
            ImGui::EndGroup();
        }
        ImGui::EndChild();

    }
    ImGui::EndChild();
}

void Menu::RenderTeleportTab() {
    ImGui::BeginChild("RightColumn", ImVec2(0, 0));
    {
        ImGui::BeginGroup();
        {
            auto width = ImGui::GetContentRegionAvail().x - 10.0f;
            auto height = 25.0f;

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, Colors::Window.Value);
            ImGui::PushStyleColor(ImGuiCol_Button, Colors::Window.Value);

            static char teleport_name[128] = "";

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 130);
            ImGui::InputText(" ", teleport_name, IM_ARRAYSIZE(teleport_name));

            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 125);

            if (ImGui::CustomButton("Save##teleport", ImVec2(105, 36.5))) {
                if (strlen(teleport_name) != 0) {
                    Teleport teleport{};
                    teleport.name = teleport_name;
                    teleport.location = util::ConvertFVectorToVector3(local_mec->get_net_location());
                    teleport.angle = vector2(local_mec->get_yaw(), local_mec->get_pitch());
                    teleport.hotkey = 0x0;

                    Settings::Teleports::teleports_cache.push_back(teleport);
                    Settings::SaveTeleports();
                    memset(teleport_name, 0, sizeof(teleport_name));
                }
            }

            ImGui::PopStyleColor(2);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, Colors::Window.Value);
            for (auto it = Settings::Teleports::teleports_cache.begin(); it != Settings::Teleports::teleports_cache.end(); ++it) {
                Teleport& teleport = *it;

                vector3 location = teleport.location;
                ImGui::PushID(&teleport);
                ImGui::BeginChild("##Teleport", ImVec2(665, 75));
                {
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 25);
                    ImGui::BeginGroup();
                    {
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
                        ImGui::PushFont(Fonts::InterSemiBold22);
                        ImGui::Text(teleport.name.c_str());
                        ImGui::PopFont();

                        ImGui::SameLine(100);
                        ImGui::AlignTextToFramePadding();

                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
                        ImGui::SetNextItemWidth(55);
                        if (ImGui::InputDouble("X", &location.x, 0, 0, "%.0f") && ImGui::IsItemDeactivatedAfterEdit()) {
                            teleport.location = location;
                        }

                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(55);
                        if (ImGui::InputDouble("Y", &location.y, 0, 0, "%.0f") && ImGui::IsItemDeactivatedAfterEdit()) {
                            teleport.location = location;
                        }

                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(55);
                        if (ImGui::InputDouble("Z", &location.z, 0, 0, "%.0f") && ImGui::IsItemDeactivatedAfterEdit()) {
                            teleport.location = location;
                        }
                        ImGui::PopStyleVar();

                        ImGui::SameLine();
                        if (ImGui::CustomButton("Telepot", ImVec2(90, 30))) {
                            local_mec->teleport(teleport.location, teleport.angle);

                            std::this_thread::sleep_for(std::chrono::milliseconds(10));

                            auto relative_location = local_mec->get_root_component()->get_relative_location();
                            auto distance = util::CalculateDistanceMeters(relative_location, teleport.location);

                            if (distance > 1.0) {
                                local_mec->teleport(teleport.location, teleport.angle);
                            }
                        }

                        ImGui::SameLine();
                        if (ImGui::CustomButton("Delete", ImVec2(90, 30))) {
                            it = Settings::Teleports::teleports_cache.erase(it);
                            Settings::SaveTeleports();
                            break;
                        }
                       
                        ImGui::SameLine();
                        int previous_hotkey = teleport.hotkey;
                        std::string hotkey_label = "Hotkey##" + teleport.name;
                        ImGui::ImHotkey(hotkey_label.c_str(), &teleport.hotkey, ImVec2(90, 30));

                        if (teleport.hotkey != previous_hotkey) {
                            Settings::SaveTeleports();
                        }
                    }
                    ImGui::EndGroup();
                }
                ImGui::EndChild();
                ImGui::PopID();
            }

            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
        }
        ImGui::EndGroup();
    }
    ImGui::EndChild();
}

void Menu::RenderMiscTab() {
    ImGui::BeginChild("LeftColumn", ImVec2(330, 0));
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, Colors::Window.Value);
        ImGui::BeginChild("lWindow1", ImVec2(0, 550));
        {
            auto width = ImGui::GetContentRegionAvail().x - 10.0f;
            auto height = 25.0f;

            ImGui::SetCursorPos(ImVec2{ ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f });
            ImGui::BeginGroup();
            {
                const int max_players = 8;
                int current_player_index = 0;

                for (auto mec : player_cache) {
                    auto mec_root = mec->get_root_component();
                    auto ghost_root = mec->get_ghost_root();
                    if (mec_root) {
                        auto state = mec->player_state();
                        if (state) {
                            auto name = state->get_player_name_private().read_string();
                            auto player_index = mec->get_old_player_index();
                            if (player_index == 1) name += " [H]";

                            vector3 mec_pos = mec_root->get_relative_location();
                            vector3 ghost_pos = ghost_root->get_relative_location();

                            vector3 teleport_pos;
                            if (!mec->get_net_alive()) {
                                ghost_pos.z -= 190.0;
                                teleport_pos = ghost_pos;
                            }
                            else {
                                teleport_pos = mec_pos;
                            }

                            if (ImGui::CustomButton(name.c_str(), ImVec2(width, height))) {
                                local_mec->teleport(teleport_pos);


                                std::this_thread::sleep_for(std::chrono::milliseconds(5));

                                auto relative_location = local_mec->get_root_component()->get_relative_location();
                                auto distance = util::CalculateDistanceMeters(relative_location, teleport_pos);

                                if (distance > 1.0) {
                                    local_mec->teleport(teleport_pos);
                                }

                            }
                            if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
                                ImGui::OpenPopup(name.c_str());
                            }

                            if (ImGui::IsPopupOpen(name.c_str())) {
                                ImGui::SetNextWindowSize(ImVec2(260, 40));
                            }

                            if (ImGui::BeginPopup(name.c_str())) {


                                ImGui::SetCursorPos(ImVec2{ ImGui::GetCursorPosX() + 10.0f, ImGui::GetCursorPosY() + 10.0f });

                                auto steam_id = state->get_data()->get_steam_id();

                                ImGui::PushStyleColor(ImGuiCol_Button, Colors::Window.Value);
                                ImGui::PushStyleColor(ImGuiCol_FrameBg, Colors::Window.Value);

                                ImGui::SetNextItemWidth(150);
                                ImGui::InputScalar(("##SteamID" + name).c_str(), ImGuiDataType_U64, &steam_id);

                                ImGui::SameLine();

                                if (ImGui::CustomButton(("Profile##" + name).c_str(), ImVec2(80, 20))) {
                                    std::string url = "https://steamcommunity.com/profiles/" + std::to_string(steam_id) + "/";
                                    HINSTANCE result = ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
                                    showMenu = false;
                                }
                                ImGui::PopStyleColor(2);
                                ImGui::EndPopup();
                            }

                            ++current_player_index;
                        }
                    }
                }

                for (int i = current_player_index; i < max_players; ++i) {
                    std::string button_text = "Player " + std::to_string(i + 1);
                    ImGui::CustomButton(button_text.c_str(), ImVec2(width, height));
                }
            }
            ImGui::EndGroup();

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            //if (ImGui::CustomButton("Show Ghosts")) {
            //    for (auto& mec : player_cache) {
            //        if (mec && !mec->get_net_alive()) {
            //            mec->get_ghost_eyes()->set_b_visible(true);
            //            mec->get_ghost()->set_b_visible(true);
            //        }
            //    }
            //}

            //if (ImGui::CustomButton("Show HackerSphere")) {
            //    for (auto& mec : player_cache) {
            //        mec->get_hacker_sphere()->set_b_visible(true);
            //    }
            //}

            //bool lockMovements = local_mec->get_lock_movements();
            //if (ImGui::CustomCheckbox("LockMovements", &lockMovements)) {
            //    local_mec->set_lock_movements(lockMovements);
            //}


            //auto canTurnCamera = local_mec->get_can_turn_camera();
            //if (ImGui::CustomCheckbox("CanTurnCamera", &canTurnCamera)) {
            //    local_mec->set_can_turn_camera(canTurnCamera);
            //}

            /*
              if (ImGui::CustomButton("Sound Log")) {
                for (auto& mec : player_cache) {
                    if (mec) {
                        auto sound_patch = mec->get_sound_patch();
                        auto state = mec->player_state();
                        if (state && sound_patch) {
                            auto source = sound_patch->get_source();
                            std::cout << "[SoundPatch] " << state->get_player_name_private().read_string()
                                << " | Max Distance: " << sound_patch->get_max_distance()
                                << " | Hear Distance: " << sound_patch->get_hear_distance()
                                //<< " | Far: " << std::boolalpha << sound_patch->get_is_far()
                                //<< " | CanHear: " << std::boolalpha << mec->get_can_hear()
                                //<< " | CanTalk: " << std::boolalpha << mec->get_can_talk()
                                << " | bVisible: " << std::boolalpha << source->get_b_visible()
                                << " | bHiddenInGame: " << std::boolalpha << source->get_hidden_in_game()
                                << std::endl;
                        }
                    }
                }
            }
            */

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);

            //auto gm_c = local_mec->get_pc_ref()->get_gm_ref();
            //auto game_time = gm_c->get_game_time();
            //if (ImGui::InputInt("Game Time", &game_time, 1, 1)) {
            //    gm_c->set_game_time(game_time);
            //}


            //auto hud = local_mec->get_hud();

            //auto blackscreen = hud->get_blackscreen();
            //auto visibility_state = blackscreen->get_visibility();
            //if (ImGui::InputInt("Visibility State", &visibility_state, 1, 1)) {
            //    blackscreen->set_visibility(visibility_state);
            //}

            //bool hud_hidden = hud->get_hud_hidden();
            //if (ImGui::CustomCheckbox("HUD Hidden", &hud_hidden)) {
            //    hud->set_hud_hidden(hud_hidden);
            //}

        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15.0f);
    ImGui::BeginChild("RightColumn", ImVec2(330, 0));
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, Colors::Window.Value);
        ImGui::BeginChild("rWindow1", ImVec2(0, 550));
        ImGui::PopStyleColor();
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
            ImGui::BeginGroup();
            {
                //static bool spam_key = false;
                //ImGui::CustomCheckbox("Spawn Key", &Settings::Misc::spamKey);

                bool admin = main_gi->get_admin();
                if (ImGui::CustomCheckbox("Admin", &admin)) {
                    main_gi->set_admin(admin);
                    main_gi->set_admin_cheats(admin);
                }

                bool admin_cheats = main_gi->get_admin_cheats();
                if (ImGui::CustomCheckbox("Admin Cheats", &admin_cheats)) {
                    main_gi->set_admin_cheats(admin_cheats);
                }

                ImGui::Spacing();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 10.0f);
                ImGui::Separator();
                ImGui::Spacing();

                t_array<fstring> admins = main_gi->get_ultimate_admins();
                std::vector<fstring> admin_list = admins.list();

                for (size_t i = 0; i < admin_list.size(); i++) {
                    ImGui::PushID(i);

                    auto temp_steam_id = std::stoull(admin_list[i].read_string());
                    if (ImGui::InputScalar(("##Admin " + std::to_string(i + 1)).c_str(), ImGuiDataType_U64, &temp_steam_id)) {
                        std::wstring wsteamID = std::to_wstring(temp_steam_id);
                        if (wsteamID.length() == 17) {
                            auto& admin = admin_list[i];
                            if (admin.count >= wsteamID.size()) {
                                memory::write(admin._data, wsteamID.data(), wsteamID.size() * sizeof(wchar_t));
                                admin.count = wsteamID.size();
                                main_gi->set_ultimate_admins(admins);
                            }
                        }
                    }

                    ImGui::SameLine();

                    if (ImGui::CustomButton(("Profile##" + std::to_string(i + 1)).c_str(), ImVec2(80, 20))) {
                        std::string url = "https://steamcommunity.com/profiles/" + admin_list[i].read_string() + "/";
                        HINSTANCE result = ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
                        showMenu = false;
                    }

                    ImGui::PopID();
                }
            }
            ImGui::EndGroup();

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::BeginGroup();
            {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
                ImGui::Text("HostTP Hotkey");
                ImGui::SameLine(ImGui::GetContentRegionAvail().x / 1.45);
                if (ImGui::ImHotkey("Hotkey##HostTP", &Settings::Misc::hostTpHotkey, ImVec2(90, 25))) {
                    Settings::SaveHotkeys();
                }

                //auto local_footstep = local_mec->get_local_footstep();
                //auto volume_multiplier = local_footstep->get_volume_multiplier();
                //if (ImGui::InputFloat("Volume Multiplier", &volume_multiplier, 1, 1)) {
                //    local_footstep->set_volume_multiplier(volume_multiplier);
                //}

                //float width = (ImGui::GetContentRegionAvail().x);
                //float half_width = width / 2;

                //if (ImGui::CustomButton("Log SteamIDs", ImVec2(half_width - 10, 25))) {
                //    for (auto mec : player_cache) {
                //        auto state = mec->player_state();
                //        if (!state) continue;

                //        auto name_str = state->get_player_name_private().read_string();
                //        uint64_t steam64 = state->get_data()->get_steam_id();

                //        std::cout << "[Lobby] Player: " << name_str << " | SteamID: " << std::dec << std::to_string(steam64) << std::endl;
                //    }
                //}

                //ImGui::SameLine();

                //if (ImGui::CustomButton("Log Admins", ImVec2(half_width - 10, 25))) {
                //    for (auto& admin : admin_list) {
                //        std::cout << "\n[UltimateAdmins] SteamID: " << admin.read_string() << std::endl << std::endl;
                //    }
                //}

                //ImGui::CustomCheckbox("FOV TP", &Settings::Misc::fovTp, false);

                //if (Settings::Misc::fovTp) {
                //   // TODO:
                //   // FOV COLOR
                //    ImGui::CustomSliderFloat("FOV", &Settings::Misc::fovTpRadius, 1.0f, 180.0f);
                //    ImGui::CustomCheckbox("Draw FOV", &Settings::Misc::drawFov);
                //    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
                //    ImGui::Text("TP Hotkey"); ImGui::SameLine(ImGui::GetContentRegionAvail().x / 1.45); ImGui::ImHotkey("Hotkey##FovTP", &Settings::Misc::fovTpHotkey, ImVec2(90, 25));
                //}
            }
            ImGui::EndGroup();
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();
}

void TextGradiented(ImDrawList* drawList, const ImVec2 pos, const char* text, ImU32 leftColor, ImU32 rightColor) {
    const ImVec2 textSize = ImGui::CalcTextSize(text);

    const ImVec2 gradientP0(pos.x, pos.y);
    const ImVec2 gradientP1(pos.x + textSize.x, pos.y);

    const int vertStartIdx = drawList->VtxBuffer.Size;
    drawList->AddText(pos, IM_COL32_WHITE, text);
    const int vertEndIdx = drawList->VtxBuffer.Size;

    ImGui::ShadeVertsLinearColorGradientKeepAlpha(drawList, vertStartIdx, vertEndIdx, gradientP0, gradientP1, leftColor, rightColor);
}

auto SettingsSubTab = SubTabs::Config;
void Menu::RenderSettingsSubTabs() {
    if (ImGui::TabButton2("Config", ImVec2(150, SubTabsHeight), SettingsSubTab == SubTabs::Config)) SettingsSubTab = SubTabs::Config;
    ImGui::SameLine();
    if (ImGui::TabButton2("Hotkeys", ImVec2(150, SubTabsHeight), SettingsSubTab == SubTabs::Hotkeys)) SettingsSubTab = SubTabs::Hotkeys;
    ImGui::SameLine();
    if (ImGui::TabButton2("Menu", ImVec2(150, SubTabsHeight), SettingsSubTab == SubTabs::Menu)) SettingsSubTab = SubTabs::Menu;
}

void Menu::RenderSettingsTab() {
    if (SettingsSubTab == SubTabs::Config) Menu::RenderConfigSubTab();
    else if (SettingsSubTab == SubTabs::Menu) Menu::RenderMenuSubTab();
    else if (SettingsSubTab == SubTabs::Hotkeys) Menu::RenderHotkeysSubTab();
}

void Menu::RenderConfigSubTab() {
    ImGui::BeginChild("LeftColumn", ImVec2(0, 0));
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));

        ImGui::PushStyleColor(ImGuiCol_FrameBg, Colors::Window.Value);
        ImGui::PushStyleColor(ImGuiCol_Button, Colors::Window.Value);

        std::filesystem::path configDir = "Configs/";

        if (!std::filesystem::exists(configDir)) {
            std::filesystem::create_directory(configDir);
            std::cout << "Config directory created." << std::endl;
        }


        std::vector<std::string> configs = Settings::GetSaveFiles(configDir);

        static char config_name[128] = "";

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 130);

        ImGui::InputText(" ", config_name, IM_ARRAYSIZE(config_name));

        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 125);

        if (ImGui::CustomButton("Save##config", ImVec2(105, 36.5))) {
            std::string filename = configDir.string() + std::string(config_name) + ".json";
            Settings::SaveSettings(filename);

            memset(config_name, 0, sizeof(config_name));
        }

        ImGui::PopStyleColor(2);

        ImGui::PushStyleColor(ImGuiCol_ChildBg, Colors::Window.Value);
        for (const auto& config : configs) {
            std::string display = Settings::StringConfigName(config);

            ImGui::BeginChild(display.c_str(), ImVec2(665, 75));
            {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 25);
                ImGui::BeginGroup(); {
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
                    ImGui::PushFont(Fonts::InterSemiBold22);
                    ImGui::Text(display.c_str());
                    ImGui::PopFont();

                    ImGui::SameLine(350);
                    if (ImGui::CustomButton("Update##config", ImVec2(90, 30))) {
                        Settings::SaveSettings(configDir.string() + config);
                    }

                    ImGui::SameLine();
                    if (ImGui::CustomButton("Delete##config", ImVec2(90, 30))) {
                        std::wstring wideConfig = util::ConvertToWideString(configDir.string() + config);

                        std::vector<std::string> updatedConfigs = Settings::GetSaveFiles(configDir);

                        if (std::filesystem::remove(wideConfig)) {
                            configs = updatedConfigs;
                        }
                    }

                    ImGui::SameLine();
                    if (ImGui::CustomButton("Load##config", ImVec2(90, 30))) {
                        Settings::LoadSettings(configDir.string() + config);
                    }
                }
                ImGui::EndGroup();
            }
            ImGui::EndChild();
        }

        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
}

void Menu::RenderMenuSubTab() {
    ImGui::BeginChild("LeftColumn", ImVec2(330, 0));
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, Colors::Window.Value);
        ImGui::BeginChild("lWindow1", ImVec2(0, 325), true);
        ImGui::PopStyleColor();
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
            ImGui::BeginGroup();
            {
                ImGuiColorEditFlags flags =
                    ImGuiColorEditFlags_AlphaBar
                    | ImGuiColorEditFlags_NoInputs
                    | ImGuiColorEditFlags_NoSidePreview
                    | ImGuiColorEditFlags_NoTooltip
                    | ImGuiColorEditFlags_PickerHueBar
                    | ImGuiColorEditFlags_NoOptions
                    | ImGuiColorEditFlags_NoInputs
                    | ImGuiColorEditFlags_NoDragDrop;


                ImGui::Spacing();

                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);

                ImGui::ColorEdit4("Menu Color", (float*)&Settings::Menu::color.Value, flags);

                ImGui::Spacing();

                ImGui::CustomCheckbox("Rainbow Color", &Settings::Menu::rainbowColor);
                ImGui::CustomSliderFloat("Rainbow Speed", &Settings::Menu::rainbowSpeed, 0.1f, 5.0f, "%.2f");
                ImGui::CustomSliderFloat("ANIM SPEED", &Settings::Menu::animSpeed, 0.65f, 1.50f, "%.2f");
            }

            ImGui::EndGroup();
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15.0f);
    ImGui::BeginChild("RightColumn", ImVec2(330, 0));
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, Colors::Window.Value);
        ImGui::BeginChild("rWindow1", ImVec2(0, 325));
        ImGui::PopStyleColor();
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
            ImGui::BeginGroup();
            {
                static ImColor LeftColor(255, 0, 0);
                static ImColor RightColor(255, 255, 255);

                ImGuiColorEditFlags flags =
                    ImGuiColorEditFlags_AlphaBar
                    | ImGuiColorEditFlags_NoInputs
                    | ImGuiColorEditFlags_NoSidePreview
                    | ImGuiColorEditFlags_NoTooltip
                    | ImGuiColorEditFlags_PickerHueBar
                    | ImGuiColorEditFlags_NoOptions;

                ImGui::ColorEdit4("LeftColor", (float*)&LeftColor, flags);
                ImGui::ColorEdit4("RightColor", (float*)&RightColor, flags);

                auto drawlist = ImGui::GetWindowDrawList();
                TextGradiented(drawlist, ImGui::GetCursorScreenPos(), "Hello, world!", LeftColor, RightColor);
            }
            ImGui::EndGroup();
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();
}

void Menu::RenderHotkeysSubTab() {

    ImGui::BeginChild("LeftColumn", ImVec2(670, 550));
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, Colors::Window.Value);
        ImGui::BeginChild("lWindow1", ImVec2(0, 0), true);
        ImGui::PopStyleColor();
        {

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
            ImGui::BeginGroup();
            {
                //ImGui::Text("Open Menu"); ImGui::SameLine(ImGui::GetContentRegionAvail().x / 1.2); ImHotkey("Open Menu", &Settings::Menu::hotkey, ImVec2(90, 20));
                //ImGui::Text("Revive/Ghost"); ImGui::SameLine(ImGui::GetContentRegionAvail().x / 1.2); ImHotkey("Revive", &Settings::Misc::reviveHotkey, ImVec2(90, 20));
                //ImGui::Text("Enable Players ESP"); ImGui::SameLine(ImGui::GetContentRegionAvail().x / 1.2); ImHotkey("Enable Players ESP", &Settings::ESP::Players::hotkey, ImVec2(90, 20));
                //ImGui::Text("Enable Items ESP"); ImGui::SameLine(ImGui::GetContentRegionAvail().x / 1.2); ImHotkey("Enable Items ESP", &Settings::ESP::Items::hotkey, ImVec2(90, 20));
                //ImGui::Text("Aimbot"); ImGui::SameLine(ImGui::GetContentRegionAvail().x / 1.2); ImHotkey("Aimbot", &Settings::Aimbot::hotkey, ImVec2(90, 20));
                //ImGui::Text("FlyHack"); ImGui::SameLine(ImGui::GetContentRegionAvail().x / 1.2); ImHotkey("FlyHack", &Settings::FlyHack::hotkey, ImVec2(90, 20));
                //ImGui::Text("FreeCam"); ImGui::SameLine(ImGui::GetContentRegionAvail().x / 1.2); ImHotkey("FreeCam", &Settings::FreeCam::hotkey, ImVec2(90, 20));
                //ImGui::Text("FlyHack[Forward]"); ImGui::SameLine(ImGui::GetContentRegionAvail().x / 1.2); ImHotkey("FlyHack[Forward]", &Settings::FlyHack::forwardKey, ImVec2(90, 20));
                //ImGui::Text("FlyHack[Left]"); ImGui::SameLine(ImGui::GetContentRegionAvail().x / 1.2); ImHotkey("FlyHack[Left]", &Settings::FlyHack::leftKey, ImVec2(90, 20));
                //ImGui::Text("FlyHack[BackWard]"); ImGui::SameLine(ImGui::GetContentRegionAvail().x / 1.2); ImHotkey("FlyHack[BackWard]", &Settings::FlyHack::backwardKey, ImVec2(90, 20));
                //ImGui::Text("FlyHack[Right]"); ImGui::SameLine(ImGui::GetContentRegionAvail().x / 1.2); ImHotkey("FlyHack[Right]", &Settings::FlyHack::rightKey, ImVec2(90, 20));
                //ImGui::Text("FlyHack[Up]"); ImGui::SameLine(ImGui::GetContentRegionAvail().x / 1.2); ImHotkey("FlyHack[Up]", &Settings::FlyHack::upKey, ImVec2(90, 20));
                //ImGui::Text("FlyHack[Down]"); ImGui::SameLine(ImGui::GetContentRegionAvail().x / 1.2); ImHotkey("FlyHack[Down]", &Settings::FlyHack::downKey, ImVec2(90, 20));

                ImVec2 windowSize = ImGui::GetWindowSize();

                ImGui::PushFont(Fonts::InterSemiBold40);

                ImGui::SetCursorPos(ImVec2(
                    (windowSize.x - ImGui::CalcTextSize("WIP").x) * 0.5f,
                    (windowSize.y - ImGui::GetFontSize()) * 0.5f
                ));

                ImGui::Text("WIP");
                ImGui::PopFont();
            }

            ImGui::EndGroup();
        }
        ImGui::EndChild();
    }   
    ImGui::EndChild();

}