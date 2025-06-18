#include "../../Modules/modules.h"
#include "../../Settings/settings.h"
#include "../../Headers/bots.h"
#include <thread>
#include <iostream>
#include "math.h"
#include "../../Headers/util.hpp"
#include <imgui/imgui_internal.h>
#include "../../Headers/Colors.hpp"


std::atomic<bool> testRunning(false);
std::thread testThread;

void Modules::Test::Enable() {
    if (testRunning) return;

    testRunning = true;

    testThread = std::thread(&Modules::Test::Thread);
    testThread.detach();
}


void Modules::Test::Disable() {
    if (!testRunning) return;

    testRunning = false;
    if (testThread.joinable()) {
        testThread.join();
    }
}

bool Modules::Test::IsRunning() {
	return testRunning;
}

const int rice_type[] = {
    100,
    200,
    300
};

const int fish_type[] = {
    10,
    20,
    30,
    40,
    50,
    60
};

const int flower_type[] = {
    1,
    2,
    3,
    4,
    5
};

FStr_ItemState GetRandomPizzushi() {

    srand(time(0));

    int rice = rice_type[rand() % 2];
    int flower = flower_type[rand() % 5];
    int fish = fish_type[rand() % 4];

    FStr_ItemState state{};
    state.Value_8 = (rice + fish + flower);

    return state;
}

void Modules::Test::Thread() {
    auto line_5 = 2;
    auto line_10 = 13;
    auto total_points = ((line_5 * 5) + (line_10 * 10));

    auto step = 100.0;

    auto pitch = -85.0;
    auto yaw = 0.0;

    auto x = 10140.0;
    auto y = 13050.0;
    auto z = 500.0;

    vector3 start_position = vector3(x, y, z);

    local_mec->teleport(start_position, vector2(yaw, pitch));

    std::this_thread::sleep_for(std::chrono::seconds(1));

    for (int i = 0; i < line_5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            local_mec->set_hand_item(unique_item_data["NAME"]);
            local_mec->set_hand_state(GetRandomPizzushi());

            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            local_mec->teleport(start_position, vector2(yaw, pitch));

            start_position.y -= step;
            std::this_thread::sleep_for(std::chrono::milliseconds(900));
        }

        start_position.y = y;
        start_position.x += step;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    for (int i = 0; i < line_10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            local_mec->set_hand_item(unique_item_data["NAME"]);
            local_mec->set_hand_state(GetRandomPizzushi());

            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            local_mec->teleport(start_position, vector2(yaw, pitch));

            start_position.y -= step;
            std::this_thread::sleep_for(std::chrono::milliseconds(900));
        }

        start_position.y = y;
        start_position.x += step;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    /*
    const ImColor curve_color = Settings::Menu::color;
    const float curve_thickness = 1.5f;

    const ImColor line_color = Colors::Mute(Colors::White, 0.70);
    const float line_thickness = 1.5f;

    const ImColor dot_color = Colors::Mute(Colors::White, 0.30);
    const float dot_radius = 5.0f;

    vector3 player_pos = Aleksey.position;

    const float head_offset = 165.0f;
    const float neck_offset = 145.0f;
    const float body_offset = 125.0f;

    vector3 target_pos = player_pos + vector3(0.0, 0.0, head_offset);
    ImVec2 windowCenter = { GetSystemMetrics(SM_CXSCREEN) / 2.0f , GetSystemMetrics(SM_CYSCREEN) / 2.0f };
    ImVec2 target_sreen_pos{};
    bool visible = util::w2s(target_pos, local_camera_manager->get_cached_frame_private().pov, target_sreen_pos);
    if (visible) {

        ImRect bb({ target_sreen_pos.x, windowCenter.y }, { windowCenter.x, target_sreen_pos.y });
        ImVec2 size = bb.GetSize();

        ImVec2 p2_offset = Settings::Aimbot::p2;
        ImVec2 p3_offset = Settings::Aimbot::p3;

        ImVec2 p1 = bb.GetTR();
        ImVec2 p4 = bb.GetBL();

        ImVec2 p2 = { bb.Min.x + p2_offset.x * size.x, bb.Min.y + p2_offset.y * size.y };
        ImVec2 p3 = { bb.Min.x + p3_offset.x * size.x, bb.Min.y + p3_offset.y * size.y };

        ImDrawList* dw = ImGui::GetBackgroundDrawList();
        //dw->AddLine(p1, p2, line_color, line_thickness);
        //dw->AddLine(p3, p4, line_color, line_thickness);

        //dw->AddBezierCubic(p1, p2, p3, p4, curve_color, curve_thickness, 32);

        //dw->AddCircleFilled(p2, dot_radius, dot_color);
        //dw->AddCircleFilled(p3, dot_radius, dot_color);

    }
  
  
    */
}