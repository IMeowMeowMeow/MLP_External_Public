#include "../../Modules/modules.h"
#include "../../Settings/Settings.h"
#include "../MLP/Headers/globals.h"
#include <thread>
#include <iostream>
#include "../../Headers/KeyManager.h"
#include "../../Headers/util.hpp"
#include "../../Headers/game_structures.hpp"
#include "../../Headers/ItemProperties.h"


std::atomic<bool> miscRunning(false);
std::thread miscThread;

void Modules::Misc::Enable() {
    if (miscRunning) return;

    miscRunning = true;

    miscThread = std::thread(&Modules::Misc::Thread);
}

void Modules::Misc::Disable() {
    if (!miscRunning) return;

    miscRunning = false;
    if (miscThread.joinable()) {
        miscThread.join();
    }
}

bool Modules::Misc::IsRunning() {
    return miscRunning;
}

void Modules::Misc::Thread() {

    while (miscRunning) {

        if (local_mec) {
            if (Settings::Player::runGun) {
                local_mec->set_run_gun(false);
            }

            if (Settings::Misc::infinityStamina && !Settings::Misc::godMode) {
                local_mec->set_stamina(0.0);
            }

            if (Settings::Misc::godMode) {
                local_mec->set_health(10000.0);
                local_mec->set_stamina(0.0);
                if (!local_mec->get_net_alive()) {
                    local_mec->set_alive(true);
                }
            }

            if (Settings::Misc::infinityFlyStamina) {
                local_mec->set_fly_stamina(1.0);
            }
            
            if (Settings::Misc::speedHack) {
                if (local_mec->get_step_handling() != Settings::Player::stepHandling) {
                    local_mec->set_step_handling(Settings::Player::stepHandling);
                }

                local_mec->set_acceleration(vector2(Settings::Player::acceleration, Settings::Player::acceleration));
            }

            auto hand_item = local_mec->get_hand_item();
            if (hand_item) {
                auto melee_item_data = (u_data_melee*)hand_item;
                auto mtype = melee_item_data->get_melee_type();
                auto item_name = hand_item->get_name().read_string();

                if (util::get_name_from_fname(hand_item->class_private()->fname_index()).find("Data_Gun_C") != std::string::npos) {

                    if (Settings::Misc::infinityAmmo) {
                        if (hand_item) {
                            auto hand_state = local_mec->get_hand_state();
                            if (hand_state.Value_8 != Settings::Misc::ammoFreezeValue) {
                                hand_state.Value_8 = Settings::Misc::ammoFreezeValue;
                                local_mec->set_hand_state(hand_state);
                            }
                        }
                    }

                    if (Settings::Misc::noSpread) {
                        auto gun_data = (u_data_gun*)hand_item;
                        if (gun_data->get_shake_intensity() != 0.0) {
                            gun_data->set_shake_intensity(0.0);

                            gun_data->set_oscillation_reactivity(0.0);
                            gun_data->set_walk_oscillation(0.0);
                            gun_data->set_run_oscillation(0.0);
                            gun_data->set_shot_oscillation(0.0);
                            gun_data->set_run_precision(0.0);
                            gun_data->set_stand_oscillation(0.0);

                            gun_data->set_recoil_reactivity(0.0);
                            gun_data->set_walk_precision(0.0);
                            gun_data->set_air_precision(0.0);
                            gun_data->set_run_precision(0.0);
                        }
                        local_mec->set_aim_oscilation_factor(0.0);
                        local_mec->set_fire_spread(0.0);
                        local_mec->set_lateral_spread(0.0);
                    }
                }
            }

            if (Settings::Misc::freezeHandItem) {
                if (!local_mec->get_hand_item()) {
                    if (Settings::Misc::saved_hand_item) {
                        local_mec->set_hand_item(Settings::Misc::saved_hand_item);
                        local_mec->set_hand_state(Settings::Misc::saved_hand_state);
                    }
                }
            }

            if (Settings::Misc::freezeBagItem) {
                if (!local_mec->get_bag_item()) {
                    if (Settings::Misc::saved_bag_item) {
                        local_mec->set_bag_item(Settings::Misc::saved_bag_item);
                        local_mec->set_bag_state(Settings::Misc::saved_bag_state);
                    }
                }
            }

            if (Settings::Player::fov != 104.0f) {
                auto camera = local_mec->get_camera();
                auto fov = camera->get_field_of_view();
                if (fov != Settings::Player::fov) {
                    camera->set_field_of_view(Settings::Player::fov);
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void Modules::Misc::SetAlive(bool TRUE_0) {
    local_mec->set_alive(TRUE_0);
    local_mec->set_health(100);
}

void Modules::Misc::SetSilentGun(bool TRUE_0) {
    static std::vector<std::string> weapons = {
        "REVOLVER",
        "SHORTY",
        "PISTOL",
        "SHOTGUN",
        "RIFLE",
        "SMG",
    };

    auto hit_drop_sound = unique_item_data["PISTOL"]->get_hit_drop_sound();

    if (TRUE_0) {
        hit_drop_sound->set_volume(0.0f);
        for (auto& weapon : weapons) {
            auto item_data = unique_item_data[weapon];
            if (!item_data) continue;

            item_data->get_use_sound()->set_volume(0.0f);

            auto gun_data = (u_data_gun*)item_data;
            gun_data->set_impact_type(4);
        }
    }
    else {
        hit_drop_sound->set_volume(1.0f);
        for (auto& weapon : weapons) {
            auto item_data = unique_item_data[weapon];
            if (!item_data) continue;

            item_data->get_use_sound()->set_volume(1.0f);

            auto item_stats = GetItemProperties(weapon);

            auto gun_data = (u_data_gun*)item_data;
            gun_data->set_impact_type(item_stats.impact_type);
        }
    }
}

void Modules::Misc::FovTP() {
    auto target = Modules::Aimbot::getNearestMec(true, Settings::Misc::fovTpRadius);
    if (target) {
        if (Settings::FlyHack::enabled) {
            Settings::FlyHack::enabled = false;
            local_mec->teleport(target);

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            Settings::FlyHack::enabled = true;
        }
        else {
            local_mec->teleport(target);
        }
    }
}

void Modules::Misc::HostTP() {
    for (auto& mec : player_cache) {
        if (!mec) continue;
        
        int player_index = mec->get_old_player_index();
        bool host = (player_index == 1);

        if (host) {
            if (Settings::FlyHack::enabled) {
                Settings::FlyHack::enabled = false;
                local_mec->teleport(mec);

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                Settings::FlyHack::enabled = true;
            }
            else {
                local_mec->teleport(mec);
            }
        }
    }
}