#include "../../Settings/settings.h"
#include "../../Modules/modules.h"
#include "../../Headers/globals.h"
#include <iostream>
#include <iomanip>
#include "../../Headers/util.hpp"
#include <fonts/fonts.h>
#include <Windows.h>
#include <unordered_map>
#include "../Menu/Gui/gui_interface.h"
#include "../../Headers/bots.h"

void Modules::ESP::Players::Draw() {

    f_camera_cache last_frame_cached = local_camera_manager->get_cached_frame_private();

    for (const auto& mec : player_cache) {
        const auto state = mec->player_state();
        if (!state) continue;

        if (mec == local_mec)  continue;

        auto mec_root = mec->get_root_component();
        if (!mec_root) continue;

        auto ghost_root = mec->get_ghost_root();
        if (!ghost_root) continue;

        auto name_str = state->get_player_name_private().read_string();
        std::wstring name_w = util::ConvertToWideString(name_str);
        std::string target_name = util::ConvertToUTF8String(name_w);
        const auto target_role = mec->get_player_role();

        vector3 target_position = mec_root->get_relative_location();
        vector3 ghost_position = ghost_root->get_relative_location();
        FVector local_position = local_mec->get_net_location();

        bool isAlive = mec->get_net_alive();

        ImColor color;
        ImColor ghostColor;
        ImColor bodyColor;

        if (target_role == E_PlayerRole::Dissident) {
            color = Settings::ESP::Colors::Dissident;
            ghostColor = Settings::ESP::Colors::DissidentGhost;
            bodyColor = Settings::ESP::Colors::DissidentBody;
        }
        else {
            color = Settings::ESP::Colors::Employee;
            ghostColor = Settings::ESP::Colors::EmployeeGhost;
            bodyColor = Settings::ESP::Colors::EmployeeBody;
        }

        if (isAlive) {
            if (Settings::ESP::Players::player) {
                auto distance = util::CalculateDistance(local_position, target_position);
                double distanceDouble = std::stod(distance);

                if (distanceDouble <= Settings::ESP::Players::maxDistance || Settings::ESP::Players::maxDistance == 0.0) {
                    ImVec2 screen_position_top, screen_position_bottom;
                    bool top_visible = util::w2s(target_position + vector3{ 0, 0, 190 }, last_frame_cached.pov, screen_position_top);
                    bool bottom_visible = util::w2s(target_position, last_frame_cached.pov, screen_position_bottom);

                    if (top_visible || bottom_visible) {

                        const float bottom_offset = 15.0f;
                        screen_position_bottom.y += bottom_offset;

                        if (Settings::ESP::Players::snapline) {
                            const ImVec2 from = Settings::ESP::Players::snaplinePos;
                            const ImVec2 to = screen_position_bottom;
                            overlay->draw_line(from, to, color);
                        }

                        if (Settings::ESP::Players::box) {
                            float box_height = screen_position_bottom.y - screen_position_top.y;
                            float box_width = box_height * 0.6f;

                            ImVec2 box_pos1 = ImVec2(screen_position_top.x - box_width * 0.5f, screen_position_top.y);
                            ImVec2 box_pos2 = ImVec2(screen_position_top.x + box_width * 0.5f, screen_position_bottom.y);

                            ImVec4 temp_color = color.Value;
                            temp_color.w = 0.05f;

                            ImU32 color_with_alpha = ImGui::ColorConvertFloat4ToU32(temp_color);
                            ImU32 corner_color = color;

                            float corner_line_length = box_width * 0.2f; // Adjust for desired length
                            float corner_thickness = 1.0f;              // Adjust for desired thickness

                            // Top-left corner lines
                            overlay->draw_rect_filled(box_pos1, ImVec2(box_pos1.x + corner_line_length, box_pos1.y + corner_thickness), corner_color);
                            overlay->draw_rect_filled(box_pos1, ImVec2(box_pos1.x + corner_thickness, box_pos1.y + corner_line_length), corner_color);

                            // Top-right corner lines
                            overlay->draw_rect_filled(ImVec2(box_pos2.x - corner_line_length, box_pos1.y), ImVec2(box_pos2.x, box_pos1.y + corner_thickness), corner_color);
                            overlay->draw_rect_filled(ImVec2(box_pos2.x - corner_thickness, box_pos1.y), ImVec2(box_pos2.x, box_pos1.y + corner_line_length), corner_color);

                            // Bottom-left corner lines
                            overlay->draw_rect_filled(ImVec2(box_pos1.x, box_pos2.y - corner_thickness), ImVec2(box_pos1.x + corner_line_length, box_pos2.y), corner_color);
                            overlay->draw_rect_filled(ImVec2(box_pos1.x, box_pos2.y - corner_line_length), ImVec2(box_pos1.x + corner_thickness, box_pos2.y), corner_color);

                            // Bottom-right corner lines
                            overlay->draw_rect_filled(ImVec2(box_pos2.x - corner_line_length, box_pos2.y - corner_thickness), box_pos2, corner_color);
                            overlay->draw_rect_filled(ImVec2(box_pos2.x - corner_thickness, box_pos2.y - corner_line_length), box_pos2, corner_color);

                            // Filled rectangle inside the box
                            ImU32 fill_color = color_with_alpha;
                            overlay->draw_rect_filled(
                                ImVec2(box_pos1.x, box_pos1.y),
                                ImVec2(box_pos2.x, box_pos2.y),
                                fill_color
                            );
                        }

                        //if (Settings::ESP::Players::skeleton) {
                        //    static int bones = 30;
                        //    auto mesh = mec->get_player_character()->mesh();
                        //    if (mesh) {
                        //        for (int i = 0; i < bones; i++) {
                        //            auto bone = mesh->get_bone(i);
                        //            ImVec2 bone_screen_pos{};
                        //            bool visible = util::w2s(bone.translation, last_frame_cached.pov, bone_screen_pos);
                        //            if (visible) {
                        //                overlay->draw_circle(bone_screen_pos, 5.0f, color);
                        //            }
                        //        }
                        //    }
        
                        //}

                        if (Settings::ESP::Players::name || Settings::ESP::Players::distance) {

                            std::string name;
                            name += Settings::ESP::Players::name ? target_name : "";
                            name += Settings::ESP::Players::distance ? " [" + distance + "m]" : "";

                            const ImVec2 text_size = ImGui::CalcTextSize(name.c_str());

                            if (!name.empty()) {
                                overlay->draw_text(ImVec2{ screen_position_bottom.x - text_size.x / 2, screen_position_bottom.y }, color, name.c_str(), true, 14);
                            }
                        }

                        if (Settings::ESP::Players::inventory) {

                            FStr_Item HandItem = mec->get_net_hand_item_new();
                            auto HandData = HandItem.Data_18;
                            FStr_ItemState HandState = HandItem.State_19;

                            std::string HandItemName = "";
                            if (HandData) HandItemName = util::GetValidName(HandData);

                            FStr_Item BagItem = mec->get_net_bag_item_new();
                            auto BagData = BagItem.Data_18;
                            FStr_ItemState BagState = BagItem.State_19;

                            std::string BagItemName = "";
                            if (BagData) BagItemName = util::GetValidName(BagData);

                            std::string inventory;
                            inventory += HandItemName;
                            
                            if (Settings::ESP::Items::state) {
                                std::string HandStateName = HandData ? Modules::ESP::Items::ConvertStateToName(HandItemName, HandState.Value_8, HandState.Time_15) : "";
                                if (!HandStateName.empty()) {
                                    inventory += " [" + HandStateName + "]";
                                }
                            }

                            inventory += (HandData && BagData) ? " | " : "";
                            inventory += BagItemName;

                            if (Settings::ESP::Items::state) {
                                std::string BagStateName = BagData ? Modules::ESP::Items::ConvertStateToName(BagItemName, BagState.Value_8, BagState.Time_15) : "";
                                if (!BagStateName.empty()) {
                                    inventory += " [" + BagStateName + "]";
                                }
                            }

                            ImVec2 text_size = ImGui::CalcTextSize(inventory.c_str());


                            if (!inventory.empty()) {
                                overlay->draw_text(ImVec2{ screen_position_bottom.x - text_size.x / 2, screen_position_bottom.y + text_size.y - 1.0f }, color, inventory.c_str(), true, 14);
                            }
                        }
                    }
                }
            }
        }
        else {
            if (Settings::ESP::Players::ghost) {

                auto distance = util::CalculateDistance(local_position, ghost_position);
                double distanceDouble = std::stod(distance);

                if (distanceDouble <= Settings::ESP::Players::maxDistance || Settings::ESP::Players::maxDistance == 0.0) {
                    ImVec2 screen_position_top, screen_position_bottom;
                    bool top_visible = util::w2s(ghost_position + vector3{ 0, 0, 20 }, last_frame_cached.pov, screen_position_top);
                    bool bottom_visible = util::w2s(ghost_position, last_frame_cached.pov, screen_position_bottom);

                    if (top_visible || bottom_visible) {
                        const float bottom_offset = 30.0f;
                        screen_position_bottom.y += bottom_offset;

                        std::string name;
                        name += Settings::ESP::Players::name ? target_name : "";
                        name += Settings::ESP::Players::distance ? " [" + distance + "m]" : "";

                        const ImVec2 name_text_size = ImGui::CalcTextSize(name.c_str());

                        if (!name.empty()) {
                            overlay->draw_text(ImVec2{ screen_position_bottom.x - name_text_size.x / 2, screen_position_bottom.y }, ghostColor, name.c_str(), true, 14);
                        }

                        if (Settings::ESP::Players::inventory) {

                            FStr_Item HandItem = mec->get_net_hand_item_new();
                            auto HandData = HandItem.Data_18;
                            FStr_ItemState HandState = HandItem.State_19;

                            std::string HandItemName = "";
                            if (HandData) HandItemName = util::GetValidName(HandData);

                            FStr_Item BagItem = mec->get_net_bag_item_new();
                            auto BagData = BagItem.Data_18;
                            FStr_ItemState BagState = BagItem.State_19;

                            std::string BagItemName = "";
                            if (BagData) BagItemName = util::GetValidName(BagData);

                            std::string inventory;
                            inventory += HandItemName;

                            if (Settings::ESP::Items::state) {
                                std::string HandStateName = HandData ? Modules::ESP::Items::ConvertStateToName(HandItemName, HandState.Value_8, HandState.Time_15) : "";
                                if (!HandStateName.empty()) {
                                    inventory += " [" + HandStateName + "]";
                                }
                            }

                            inventory += (HandData && BagData) ? " | " : "";
                            inventory += BagItemName;

                            if (Settings::ESP::Items::state) {
                                std::string BagStateName = BagData ? Modules::ESP::Items::ConvertStateToName(BagItemName, BagState.Value_8, BagState.Time_15) : "";
                                if (!BagStateName.empty()) {
                                    inventory += " [" + BagStateName + "]";
                                }
                            }

                            ImVec2 text_size = ImGui::CalcTextSize(inventory.c_str());


                            if (!inventory.empty()) {
                                overlay->draw_text(ImVec2{ screen_position_bottom.x - text_size.x / 2, screen_position_bottom.y + text_size.y - 1.0f }, color, inventory.c_str(), true, 14);
                            }
                        }
                    }
                }
            }

            if (Settings::ESP::Players::deadBody) {

                auto distance = util::CalculateDistance(local_position, target_position);
                double distanceDouble = std::stod(distance);

                if (distanceDouble <= Settings::ESP::Players::maxDistance || Settings::ESP::Players::maxDistance == 0.0) {

                    ImVec2 screen_position_bottom;
                    bool bottom_visible = util::w2s(target_position, last_frame_cached.pov, screen_position_bottom);

                    if (bottom_visible) {

                        const float bottom_offset = 15.0f;
                        screen_position_bottom.y += bottom_offset;

                        std::string name;
                        name += Settings::ESP::Players::name ? target_name + " [X]" : "";
                        name += Settings::ESP::Players::distance ? " [" + distance + "m]" : "";

                        const ImVec2 text_size = ImGui::CalcTextSize(name.c_str());

                        if (!name.empty()) {
                            overlay->draw_text(ImVec2{ screen_position_bottom.x - text_size.x / 2, screen_position_bottom.y }, bodyColor, name.c_str(), true, 14);
                        }
                    }
                }
            }
        }
    }
}