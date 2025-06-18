#include "../../Memory/memory.hpp"
#include "../../Settings/settings.h"
#include "../../Modules/modules.h"
#include "../../Headers/globals.h"
#include <iostream>
#include <chrono>
#include <thread>
#include "math.h"
#include <cmath>
#include "../../Headers/util.hpp"
#include <filesystem>
#include "../../Headers/KeyManager.h"
#include "../Menu/Gui/gui_interface.h"
#include "../../Headers/bots.h"

using namespace globals;

std::atomic<bool> aimbotRunning(false);
std::thread aimbotThread;

vector2 windowCenter = { GetSystemMetrics(SM_CXSCREEN) / 2.0f, GetSystemMetrics(SM_CYSCREEN) / 2.0f };

ImVec2 BezierCubicCalc(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, float t)
{
    float u = 1.0f - t;
    float w1 = u * u * u;
    float w2 = 3 * u * u * t;
    float w3 = 3 * u * t * t;
    float w4 = t * t * t;
    return ImVec2(w1 * p1.x + w2 * p2.x + w3 * p3.x + w4 * p4.x, w1 * p1.y + w2 * p2.y + w3 * p3.y + w4 * p4.y);
}

#define min(a, b) a < b ? a : b

void Modules::Aimbot::Enable() {
    if (aimbotRunning) return;

    aimbotRunning = true;

    aimbotThread = std::thread(&Modules::Aimbot::Thread);
}

void Modules::Aimbot::Disable() {
    if (!aimbotRunning) return;

    aimbotRunning = false;
    if (aimbotThread.joinable()) {
        aimbotThread.join();
    }
}

bool Modules::Aimbot::IsRunning() {
    return aimbotRunning;
}


void Modules::Aimbot::Thread() {
    while (aimbotRunning) {
        if (KeyManager::IsKeyPressed(Settings::Aimbot::hotkey) && game_window_active) {


            mec_pawn* target = Modules::Aimbot::getNearestMec(Settings::Aimbot::checkFOV, Settings::Aimbot::fov);
            if (!target) {
                continue;
            }

            vector3 local_pos = local_mec->get_root_component()->get_relative_location();
            vector3 target_pos = target->get_root_component()->get_relative_location();
            vector2 angle = util::CalcAngle(local_pos, target_pos);

            local_mec->set_pitch(angle.y);
            local_mec->set_yaw(angle.x);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}


void Modules::Aimbot::DrawFOV(float fov_radius) {

    overlay->draw_circle(ImVec2(windowCenter.x, windowCenter.y), fov_radius * 5, ImColor(255, 255, 255, 200), 128, 1.5f);

    // Draw a crosshair
    //ImGui::GetBackgroundDrawList()->AddLine(ImVec2(windowCenter.x - 10, windowCenter.y), ImVec2(windowCenter.x + 10, windowCenter.y), ImColor(255, 0, 0), 2.0f);
    //ImGui::GetBackgroundDrawList()->AddLine(ImVec2(windowCenter.x, windowCenter.y - 10), ImVec2(windowCenter.x, windowCenter.y + 10), ImColor(255, 0, 0), 2.0f);
}

bool Modules::Aimbot::IsInFOV(ImVec2& screen_pos, float fov_radius) {
    screen_pos.x += game_rect.left;
    screen_pos.y += game_rect.top;

    return windowCenter.distance(screen_pos) <= (fov_radius) * 5;
}

std::string aim_target = "Head";
mec_pawn* Modules::Aimbot::getNearestMec(bool check_fov, float fov_radius) {
    double oldDistance = DBL_MAX;
    double newDistance = 0.0;
    mec_pawn* nearestMec = nullptr;

    auto local_mec_pos = local_mec->get_net_location();

    for (const auto& mec : player_cache) {

        auto state = mec->player_state();
        if (!state) continue;

        if (mec != local_mec) {
            auto mec_root = mec->get_root_component();
            if (!mec_root) continue;

            auto ghost_root = mec->get_ghost_root();
            if (!ghost_root) continue;

            vector3 player_position = mec_root->get_relative_location();
            vector3 ghost_position = ghost_root->get_relative_location();

            const bool isAlive = mec->get_net_alive();
            if (!isAlive) continue;

            auto distance = util::CalculateDistance(local_mec_pos, player_position);
            double distanceDouble = std::stod(distance);

            if (distanceDouble > Settings::Aimbot::maxDistance && Settings::Aimbot::maxDistance != 0.0) continue;

            if (mec->get_player_role() == E_PlayerRole::Dissident && local_mec->get_player_role() == E_PlayerRole::Dissident) continue;

            float aim_height_offset = (aim_target == "Neck") ? 145.0f : (aim_target == "Body") ? 125.0f : 165.0f;


            vector3 target_position = player_position + vector3(0.0, 0.0, aim_height_offset);

            ImVec2 target_sreen_pos{};
            if (util::w2s(target_position, local_camera_manager->get_cached_frame_private().pov, target_sreen_pos)) {

                if (check_fov && !Modules::Aimbot::IsInFOV(target_sreen_pos, fov_radius)) continue;

                newDistance = abs(windowCenter.distance(target_sreen_pos));
                if (newDistance < oldDistance) {
                    oldDistance = newDistance;
                    nearestMec = mec;
                }
            }
        }
    }

    return nearestMec;
}