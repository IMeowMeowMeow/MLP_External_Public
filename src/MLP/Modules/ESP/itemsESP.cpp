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
#include <unordered_set>

void Modules::ESP::Items::Draw() {
    f_camera_cache last_frame_cached = local_camera_manager->get_cached_frame_private();

    //
    //    for (auto weapon_case : weapon_case_cache) {
    //    if (!weapon_case) continue;

    //    auto role = local_mec->get_player_role();
    //    if (role == 3 || role == 4) {
    //        // get case esp bool
    //        ImU32 case_color = IM_COL32(255, 255, 125, 255);

    //        auto weapon_case_code = weapon_case->get_target_values();
    //        auto case_code = weapon_case_code.list();
    //        auto case_timer = weapon_case->get_opening_timer().Handle;
    //        auto open_delay = weapon_case->get_open_delay();

    //        auto box_to_open = weapon_case->get_box_to_open();
    //        auto case_weapon = box_to_open->get_selected_weapon_qsdsf();
    //        auto case_open = box_to_open->get_case_open();
    //        //if (!case_weapon) continue;

    //        auto case_weapon_state = box_to_open->get_item_slot();
    //        auto case_weapon_ammo = case_weapon_state->get_item_state().Value_8;

    //        // Build the string
    //        std::string weapon_case_code_string;
    //        std::ostringstream oss;

    //        for (const auto& byte : case_code) {
    //            oss << static_cast<int>(byte); // Convert UINT8 to integer and append directly
    //        }

    //        weapon_case_code_string = oss.str(); // Extract the string

    //        auto caseRoot = weapon_case->get_root_component();
    //        if (!caseRoot) {
    //            std::cout << "weapon case root is null!" << std::endl;
    //            continue;
    //        }

    //        auto caseLocation = caseRoot->get_relative_location();
    //        auto selected_weapon = box_to_open->get_selected_weapon();
    //        std::unordered_map<int, std::string> weapon_map = {
    //            {1, "Empty"},
    //            {2, "Pistol"},
    //            {3, "Revolver"},
    //            {4, "Shorty"}
    //        };

    //        std::string case_weapon_name = weapon_map.count(selected_weapon) ? weapon_map[selected_weapon] : "Unknown";

    //        if (!case_open && selected_weapon != 1) {
    //            auto distance = util::CalculateDistance(local_mec->get_net_location(), caseLocation);
    //            double distanceDouble = std::stod(distance);

    //            ImVec2 screen_position{};
    //            if (util::w2s(caseLocation, last_frame_cached.pov, screen_position)) {
    //                // Calculate text width based on character count and font size
    //                std::string name_norm = "[CASE]";
    //                int text_width = name_norm.length() * 7; // Assume each character is approximately 6 pixels wide
    //                screen_position.x -= text_width / 2; // Shift the position left by half the text width

    //                overlay->draw_text(screen_position, case_color, name_norm.c_str(), true);
    //                screen_position.y += 15;
    //                std::string weapon_case_text = "[Code: " + weapon_case_code_string + "]";
    //                overlay->draw_text(screen_position, case_color, weapon_case_text.c_str(), true);
    //                screen_position.y += 15;
    //                weapon_case_text = "[" + case_weapon_name + "|Ammo:" + std::to_string(case_weapon_ammo) + "]";
    //                overlay->draw_text(screen_position, case_color, weapon_case_text.c_str(), true);

    //                screen_position.y += 15;
    //                std::string case_timer_text = "[" + std::to_string(open_delay) + "]";
    //                overlay->draw_text(screen_position, case_color, case_timer_text.c_str(), true);
    //            }
    //        }
    //    }
    //}

    for (const auto& item : world_item_cache) {
        if (!item) continue;

        const auto data = item->get_data();
        auto item_name = util::GetValidName(data);

        if (!IsSelectedItem(item_name)) continue;

        const auto item_root = item->get_root_component();
        if (!item_root) continue;

        auto position = item_root->get_relative_location();

        /*
        std::cout << "X:" << position.x << std::endl;
        std::cout << "Y:" << position.y << std::endl;
        std::cout << "Z:" << position.z << std::endl << std::endl;
        */

        if (position.y > 14500.0 || position.x == 0.0) continue;

        auto local_pos = local_mec->get_net_location();
        auto distance = util::CalculateDistance(local_pos, position);

        double distanceDouble = std::stod(distance);
        if (distanceDouble <= Settings::ESP::Items::maxDistance || Settings::ESP::Items::maxDistance == 0.0) {

            ImVec2 screen_position{};
            if (util::w2s(position, last_frame_cached.pov, screen_position)) {

                std::string name;
                name += Settings::ESP::Items::name ? item_name : "";
                name += Settings::ESP::Items::distance ? " [" + distance + "m]" : "";

                if (Settings::ESP::Items::state) {
                    const auto& item_state = item->get_item_state();
                    std::string state_name = ConvertStateToName(item_name,
                        item_state.Value_8,
                        item_state.Time_15);
                    if (!state_name.empty()) {
                        name += " [" + state_name + "]";
                    }
                }

                ImColor color = GetItemColor(item_name);
                ImVec2 text_size = ImGui::CalcTextSize(name.c_str());

                overlay->draw_text(ImVec2{ screen_position.x - text_size.x / 2, screen_position.y }, color, name.c_str(), true, 14);
            }
        }
    }
}

std::string Modules::ESP::Items::ConvertStateToName(const std::string& item_name, int state_value, int time_value) {
    static const std::unordered_set<std::string> ExcludedItems = {
        "PIZZUSHI", "DETONATOR", "C4", "KNIFE", "SCREW DRIVER", "DEFAULT MELEE", "HARD DRIVE", "ACCESS CARD"
    };

    if (!ExcludedItems.contains(item_name)) {
        if (item_name == "GAZ BOTTLE") {
            const char* colors[] = { "Yellow", "Red", "Blue", "Yellow 4", "Black" };
            if (state_value < 0 || state_value > 4) return colors[4];
            else return colors[state_value];
        }
        else if (item_name == "PACKAGE") {
            const char* package_types[] = { "Camera", "Camera", "Computers", "Botanic", "Restaurant", "Medical", "Tutorial" , "Machine", "0" };
            if (state_value < 0 || state_value > 7) return package_types[8];
            else return package_types[state_value];
        }
        else if (item_name == "FISH") {
            const char* fish_types[] = { "Unknown", "Salmon", "Tuna", "Cod", "Shrimp" };
            if (state_value < 0 || state_value > 5) return fish_types[0];
            else return fish_types[state_value];
        }
        else if (item_name == "RICE") {
            const char* rice_types[] = { "White", "Brown", "Black" };
            if (state_value < 1 || state_value > 3) return rice_types[0];
            else return rice_types[state_value - 1];
        }
        else if (item_name == "CASSETTE") {
            const char* songs[] = { "KHARMA", "Who Am I", "Burning Wish", "Cake", "Puzzle", "Sun", "Worship", "Royalty (Instrumental)", "GRAVE", "Unknown" };
            if (state_value < 0 || state_value > 8) return songs[9];
            else return songs[state_value];
        }
        else if (item_name == "CONTAINER") {
            const char* container_states[] = { "Empty", "Green", "Yellow", "Blue", "White", "Red", "White Rice", "Brown Rice", "Black Rice" , "Dirty" };
            if (state_value < 0) return container_states[9];
            else if (state_value > 8) return container_states[0];
            else if (state_value == 6) return container_states[(state_value - 1) + time_value];
            else return container_states[state_value];
        }
        else if (item_name == "SAMPLE") {
            const char* value_colors[] = { "Empty", "Green", "Yellow", "Blue", "White", "Red" };
            const char* time_colors[] = { "Empty", "Red" };

            std::string result;

            if (state_value < 0 || state_value > 5) result = value_colors[2];
            else result = value_colors[state_value];

            if (time_value >= 1 && state_value < 5) {
                result += " + " + std::string(time_colors[1]);
            }

            return result;
        }
        else if (item_name == "FUSE") {
            const char* value_colors[] = { "White", "Red", "Yellow", "Blue", "Black" };
            const char* time_colors[] = { "White", "Red", "Yellow", "Blue", "Black" };
            std::string result = "Unknown";

            if (state_value < 0 || state_value >= 4) result = value_colors[4];
            else result = value_colors[state_value];

            if (time_value < 0 || time_value >= 4) result += "/" + std::string(time_colors[4]);
            else result += "/" + std::string(time_colors[time_value]);

            return result;
        }
        else if (item_name == "BATTERY" || item_name == "VENT FILTER" || item_name == "DEFIBRILLATOR") {
            return std::to_string(state_value) + "%";
        }
        else {
            return std::to_string(state_value);
        }
    }

    return "";
}

bool Modules::ESP::Items::IsSelectedItem(const std::string& item_name) {
    auto is_selected = [&](const auto& entry) {
        return entry.second && entry.first == item_name;
    };

    return std::ranges::any_of(Settings::ESP::Items::weapons_map, is_selected) ||
        std::ranges::any_of(Settings::ESP::Items::primary_map, is_selected) ||
        std::ranges::any_of(Settings::ESP::Items::secondary_map, is_selected);
}

ImColor Modules::ESP::Items::GetItemColor(const std::string& itemName) {
    auto it = Settings::ESP::Colors::ColorsMap.find(itemName);
    return (it != Settings::ESP::Colors::ColorsMap.end()) ? it->second : ImColor(255, 255, 255);
}