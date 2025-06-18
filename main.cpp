#include <windows.h>
#include <wtypes.h>
#include "../Menu/menu.h"
#include "../MLP/Modules/modules.h"
#include "../MLP/Settings/settings.h"
#include "../MLP/Headers/globals.h"
#include "../MLP/Headers/KeyManager.h"
#include "../MLP/Headers/game_structures.hpp"
#include "../MLP/Headers/game_function.hpp"
#include "../MLP/Headers/ItemProperties.h"
#include "../MLP/Headers/util.hpp"
#include "../MLP/Headers/enums.h"
#include <unordered_map>
#include <unordered_set>
#include <codecvt>
#include <chrono>
#include <sstream>
#include <iostream>
#include <string>
#include <thread>
#include <iomanip>
#include <print>
#include <math.h>

#include "src/Menu/Gui/gui_interface.h"
#include "src/MLP/Headers/widget.hpp"
using namespace globals;
using namespace protocol::engine::sdk;
using namespace protocol::game::sdk;
using namespace protocol::engine;

std::unordered_map<std::string, ItemProperties> itemData;

void InitializeItems() {
    itemData["REVOLVER"] = ItemProperties::CreateGun(25, 25, 0.50, 80, 20, 6, 0.3, 15, 2, 0, false, true);
    itemData["PISTOL"] = ItemProperties::CreateGun(5, 30, 0.80, 15, 60, 12, 0.15, 8, 1, 0, false, true);
    itemData["SHORTY"] = ItemProperties::CreateGun(4, 8, 0.50, 10, 10, 4, 0.4, 25, 0, 1, false, true);
    itemData["SHOTGUN"] = ItemProperties::CreateGun(5, 5, 0.25, 10, 5, 10, 0.2, 8, 0, 2, true, false);
    itemData["RIFLE"] = ItemProperties::CreateGun(25, 10, 0.70, 60, 40, 24, 0.12, 3, 2, 0, true, false);
    itemData["SMG"] = ItemProperties::CreateGun(10, 30, 0.75, 15, 60, 48, 0.07, 1, 1, 0, true, false);
    itemData["DEFAULT GUN"] = ItemProperties::CreateGun(2, 2, 0.5, 6, 2, 10, 0.01, 2, 0, 0, false, true);

    itemData["GAZ BOTTLE"] = ItemProperties::CreateMelee(10, 60, -1, 10, 90, -1, 180, 1.0, 40, 0.30, false);
    itemData["VENT FILTER"] = ItemProperties::CreateMelee(10, 60, -1, 10, 90, -1, 180, 1.0, 40, 0.30, false);
    itemData["PACKAGE"] = ItemProperties::CreateMelee(10, 60, -1, 10, 90, -1, 180, 1.0, 40, 0.30, false);
    itemData["RICE"] = ItemProperties::CreateMelee(10, 60, -1, 10, 90, -1, 180, 1.0, 40, 0.30, false);
    itemData["PIZZUSHI"] = ItemProperties::CreateMelee(10, 60, -1, 10, 90, -1, 180, 1.0, 40, 0.30, false);
    itemData["MACHINE PART A"] = ItemProperties::CreateMelee(20, 5, 0.10, 20, 20, 0.40, 130, 0.40, 15, 0.10, true);
    itemData["MACHINE PART B"] = ItemProperties::CreateMelee(5, 35, -0.8, 20, 80, 0.90, 130, 0.70, 20, 0.20, true);
    itemData["MACHINE PART C"] = ItemProperties::CreateMelee(10, 60, -1, 10, 90, -1, 180, 1.0, 40, 0.30, false);
    itemData["MACHINE PART D"] = ItemProperties::CreateMelee(5, 35, -0.8, 20, 80, 0.90, 130, 0.70, 20, 0.20, true);

    itemData["FUSE"] = ItemProperties::CreateMelee(5, 35, -0.8, 20, 80, 0.9, 130, 0.70, 20, 0.20, true);
    itemData["CASSETTE"] = ItemProperties::CreateMelee(5, 35, -0.8, 20, 80, 0.9, 130, 0.70, 20, 0.20, true);
    itemData["BATTERY"] = ItemProperties::CreateMelee(5, 35, -0.8, 20, 80, 0.9, 130, 0.70, 20, 0.20, true);
    itemData["CONTAINER"] = ItemProperties::CreateMelee(5, 35, -0.8, 20, 80, 0.9, 130, 0.70, 20, 0.20, true);
    itemData["C4"] = ItemProperties::CreateMelee(5, 35, -0.8, 20, 80, 0.9, 130, 0.70, 20, 0.20, true);
    itemData["SAMPLE"] = ItemProperties::CreateMelee(5, 35, -0.8, 20, 80, 0.9, 130, 0.70, 20, 0.20, true);
    itemData["DETONATOR"] = ItemProperties::CreateMelee(5, 35, -0.8, 20, 80, 0.9, 130, 0.70, 20, 0.20, true);
    itemData["HARD DRIVE"] = ItemProperties::CreateMelee(5, 35, -0.8, 20, 80, 0.9, 130, 0.70, 20, 0.20, true);
    itemData["EGG"] = ItemProperties::CreateMelee(5, 35, -0.8, 20, 80, 0.9, 130, 0.70, 20, 0.20, true);

    itemData["DEFAULT MELEE"] = ItemProperties::CreateMelee(5, 35, -0.8, 20, 80, 0.9, 130, 0.70, 20, 0.20, true);
    itemData["DEFIBRILLATOR"] = ItemProperties::CreateMelee(10, 30, -1, 20, 40, -1, 180, 1.0, 40, 0.30, false);
    itemData["FISH"] = ItemProperties::CreateMelee(20, 80, 1, 20, 80, 1, 130, 0.70, 20, 0.20, false);
    itemData["KNIFE"] = ItemProperties::CreateMelee(40, 20, 0.40, 70, 20, 0.60, 130, 0.40, 20, 0.10, true);
    itemData["SCREW DRIVER"] = ItemProperties::CreateMelee(15, 5, 0.10, 20, 30, 0.40, 130, 0.40, 15, 0.10, true);
}

void InitializeProccessEvent() {
    uintptr_t process_event_address = memory::module_base + protocol::engine::PROCESSEVENT;
    globals::process_event = reinterpret_cast<tProcessEvent>(process_event_address);

    if (globals::process_event) {
        std::cout << "'ProcessEvent' successfully initialized at: " << std::hex << process_event_address << std::endl;
    }
    else {
        std::cerr << "Failed to initialize 'ProcessEvent'!" << std::endl;
    }
}

ItemProperties GetItemProperties(const std::string& itemName) {
    if (itemData.find(itemName) != itemData.end()) {
        return itemData[itemName];
    }

    return ItemProperties();
}

static bool initialized = false;
static void cache_useful() {

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
        gworld = u_world::get_world(memory::module_base);
        if (!gworld) continue;
        game_state = gworld->get_game_state();
        if (!game_state) continue;
        owning_instance = gworld->get_owning_game_instance();
        if (!owning_instance) continue;
        main_gi = reinterpret_cast<u_main_gi_c*>(owning_instance);
        local_player = owning_instance->get_localplayer();
        if (!local_player) continue;
        local_controller = local_player->get_player_controller();
        if (!local_controller) continue;
        local_camera_manager = local_controller->get_camera_manager();
        if (!local_camera_manager) continue;
        local_mec = (mec_pawn*)local_controller->get_pawn();
        if (!local_mec) continue;

        std::vector < mec_pawn* > temp_player_cache{};
        std::vector < world_item* > temp_world_item_cache{};
        //std::vector < a_item* > temp_item_cache{};
        std::vector < a_interaction_c* > temp_interaction_cache{};
        std::vector < a_weapon_case_code_c* > temp_weapon_case_cache{};

        auto levels = gworld->get_levels();
        for (auto level : levels.list()) {
            auto actors = level->get_actors();

            for (auto actor : actors.list()) {
                auto class_name = util::get_name_from_fname(actor->class_private()->fname_index());
                auto name = util::get_name_from_fname(actor->outer()->fname_index());
                // std::cout << class_name << std::endl;
                if (class_name.find("WorldItem_C") != std::string::npos) {
                    auto item = static_cast<world_item*>(actor);
                    temp_world_item_cache.push_back(item);
                }

                //if (class_name.find("Item_C") != std::string::npos) {
                //    auto item = static_cast<a_item*>(actor);
                //    temp_item_cache.push_back(item);
                //}

                if (class_name.find("Mec_C") != std::string::npos) {
                    temp_player_cache.push_back((mec_pawn*)actor);
                }
                if (class_name.find("WeaponCaseCode_C") != std::string::npos) {
                    temp_weapon_case_cache.push_back((a_weapon_case_code_c*)actor);
                }
                if (class_name.find("Interaction_C") != std::string::npos) {
                    temp_interaction_cache.push_back((a_interaction_c*)actor);
                }
            }
        }
        player_cache = temp_player_cache;
        world_item_cache = temp_world_item_cache;
        //item_cache = temp_item_cache;
        interaction_cache = temp_interaction_cache;
        weapon_case_cache = temp_weapon_case_cache;

        if (!initialized && !temp_world_item_cache.empty()) {
            //InitializeProccessEvent();
            InitializeItems();

            auto admin_items = local_mec->get_admin_items();
            for (auto& item : admin_items.list()) {
                auto item_data = item.Data_18;
                auto item_name = item_data->get_name().read_string();
                unique_item_data[item_name] = item_data;
                std::cout << item_name << std::endl;
            }

            auto debug_items = local_mec->get_debug_items();
            for (auto& item : debug_items.list()) {
                auto item_data = item.Data_18;
                auto item_name = item_data->get_name().read_string();
                unique_item_data[item_name] = item_data;
                std::cout << item_name << std::endl;
            }
            
            //for (auto& item : item_cache) {
            //    auto data = item->get_item_data();
            //    auto name = data->get_name().read_string();
            //    item->set_can_sound(false);
            //    item->set_place_type(2);
            //    std::cout << "[AItem_C][" << std::hex << (uintptr_t)item << "] " << name <<  std::endl;
            //}


            //nopush_addr = memory::aob(memory::process, NoPushPattern, NoPushMask, memory::module_base, 0x4000000);
            nofall_addr = memory::aob(memory::process, NoFallPattern, NoFallMask, memory::module_base, 0x4000000);

            //camera1_addr = memory::aob(memory::process, Camera1Pattern, CameraMask, memory::module_base, 0x4000000);
            //camera2_addr = memory::aob(memory::process, Camera2Pattern, CameraMask, memory::module_base, 0x4000000);
            //noclip_addr = memory::aob(memory::process, NoClipPattern, NoClipMask, memory::module_base, 0x4000000);

            //std::cout << "Camera1 address: " << std::hex << camera1_addr << std::endl;
            //std::cout << "Camera2 address: " << std::hex << camera2_addr << std::endl;
            
           //std::vector<std::string> Tobject_names = {
           //    "Data_Skin_Body_C",
           //};

           //for (auto& object_name : Tobject_names) {
           //     auto object = find_object(object_name);
           //}dddd

            std::vector<std::string> item_objects = {
                //"DA_Shorty",
                //"DA_Revolver",
                //"DA_Pistol",
                //"DA_Shotgun",
                //"DA_SMG",
                //"DA_Rifle",
                //"DA_Detonator",
                //"DA_C4",
                //"DA_Fish",
                //"DA_Pizzushi",
                "DA_HardDrive",
                //"DA_Wrench",
                //"DA_Seringe",
                //"DA_Bottle",
                //"DA_SMGDefault",
                //"DA_PistolDefault",
                //"DA_ShotgunDefault",
                //"DA_ShortyDefault",
                //"DA_RevolverDefault",
                //"DA_RifleDefault",
                "Default__Data_Gun_C",
                "Default__Data_Melee_C",
                "DA_MachinePart_A",
                "DA_MachinePart_B",
                "DA_MachinePart_C",
                "DA_MachinePart_D",
                //"DA_Package",
                //"Default__Data_Consumable_C"
            };

            for (auto& object_name : item_objects) {
                auto item_data = AssignToItemData(object_name);
                if (!item_data) continue;

                std::string item_name;
                if (object_name == "Default__Data_Gun_C") item_name = "DEFAULT GUN";
                else if (object_name == "Default__Data_Melee_C") item_name = "DEFAULT MELEE";
                else if (object_name == "Default__Data_Consumable_C") item_name = "DEFAULT CONSUMABLE";
                else if (object_name == "DA_MachinePart_A") item_name = "MACHINE PART A";
                else if (object_name == "DA_MachinePart_B") item_name = "MACHINE PART B";
                else if (object_name == "DA_MachinePart_C") item_name = "MACHINE PART C";
                else if (object_name == "DA_MachinePart_D") item_name = "MACHINE PART D";
                else item_name = item_data->get_name().read_string();

                unique_item_data[item_name] = item_data;
                std::cout << item_name << std::endl;
            }

            std::vector<std::string> sound_objects = {
                "MS_Step_Jog",
                "MS_Step_Run",
                "MS_Step_Walk",
                "MS_Step_Jump",
                "MS_Step_Ninja",
                "MS_LightFall",
                "MS_Fall",
                "MS_DetonationEcho",
            };
            
            for (auto& object_name : sound_objects) {
                auto obj = find_object(object_name);
            }

            std::cout << std::hex << std::uppercase << (uintptr_t)local_mec << std::endl;

            initialized = true;
        }
    }
}

static void item_data_cache() {
    while (true) {
        if (!world_item_cache.empty()) {
            for (auto item : world_item_cache) {
                if (!item) continue;

                auto item_root = item->get_root_component();
                if (!item_root) continue;

                auto item_data = item->get_data();
                if (!item_data) continue;

                auto item_name = item_data->get_name().read_string();

                if (unique_item_data.find(item_name) == unique_item_data.end()) {
                    unique_item_data[item_name] = item_data;
                    std::cout << item_name << std::endl;
                }
                else if (initialized) {
                    auto exist_data = unique_item_data[item_name];
                    auto exist_name = exist_data->get_name().read_string();
                    if (exist_name != item_name) {
                        std::cout << "[UPDATED] " << item_name << std::endl;
                        unique_item_data[item_name] = item_data;
                    }
                }
            }

            double ping_timer = local_mec->get_ping_timer();
            if (ping_timer < 2 && initialized) {
                std::unordered_map<std::string, std::string> object_names = {
                    {"DA_HardDrive", "HARD DRIVE"},
                    {"Default__Data_Gun_C", "DEFAULT GUN"},
                    {"Default__Data_Melee_C","DEFAULT MELEE"},
                    {"DA_MachinePart_A", "MACHINE PART A"},
                    {"DA_MachinePart_B", "MACHINE PART B"},
                    {"DA_MachinePart_C", "MACHINE PART C"},
                    {"DA_MachinePart_D", "MACHINE PART D"},
                };

                for (auto& [object_name, item_name] : object_names) {
                    auto exist_data = unique_item_data[item_name];
                    auto exist_name = util::get_name_from_fname(exist_data->fname_index());

                    if (exist_name.find(object_name) != std::string::npos) continue;

                    auto item_data = AssignToItemData(object_name);
                    if (!item_data) continue;

                    std::cout << "[UPDATED] " << item_name << std::endl;
                    unique_item_data[item_name] = item_data;
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void render_callback() {
    Menu::Draw();
}

void ModulesThread() {
    Modules::Handler::Enable();
}

int current_skin_color = 1;
void SkinThread() {
    while (true) {
        if (Settings::Misc::rainbowSkin) {
            auto skin_set = local_mec->get_skin_set();
            skin_set.Color_8 = current_skin_color;
            local_mec->set_skin_set(skin_set);

            current_skin_color = (current_skin_color % 9) + 1;

        }
        std::this_thread::sleep_for(std::chrono::milliseconds(Settings::Misc::rainbowSkinDelay));
    }
}

void HotKeysThread()
{
    while (true) {
    
        HWND overlay_window = overlay->get_window_handle();
        auto foreground_window = GetForegroundWindow();

        game_window_active = (foreground_window == game_window || foreground_window == overlay_window);

        if (KeyManager::IsKeyToggled(VK_END)) {
            ExitProcess(0);
        }


        if (KeyManager::IsKeyPressed(VK_MENU) || !game_window_active) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        for (auto& [label, hotkey] : Hotkeys::HotkeysMap) {
            int key = hotkey.key;
            HotkeyMode mode = hotkey.mode;

            if (mode == HotkeyMode::Hold)
            {
                if (KeyManager::IsKeyToggled(key))
                {
                    if (hotkey.onActivate) {
                        hotkey.onActivate();
                    }
                }

                if (KeyManager::IsKeyReleased(key))
                {
                    if (hotkey.onDeactivate) {
                        hotkey.onDeactivate();
                    }
                }
            }
            else if (mode == HotkeyMode::Toggle)
            {
                if (KeyManager::IsKeyToggled(key))
                {
                    if (hotkey.onActivate) {
                        hotkey.onActivate();
                    }
                }
            }
        }

        for (const auto& teleport : Settings::Teleports::teleports_cache) {
            if (teleport.hotkey != 0 && KeyManager::IsKeyToggled(teleport.hotkey)) {
                local_mec->teleport(teleport.location, teleport.angle);

                std::this_thread::sleep_for(std::chrono::milliseconds(10));

                auto relative_location = local_mec->get_root_component()->get_relative_location();
                auto distance = util::CalculateDistanceMeters(relative_location, teleport.location);

                if (distance > 1.0) {
                    local_mec->teleport(teleport.location, teleport.angle);
                }
            }
        }
        
        if (KeyManager::IsKeyToggled(Settings::Misc::hostTpHotkey)) {
            Modules::Misc::HostTP();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void CheckGameProcess() {
    while (true) {
        HWND hwnd = FindWindow(NULL, L"LOCKDOWN Protocol  "); 
        if (!hwnd) {;
            ExitProcess(0);
        }

        if (!game_window_active) {
            Menu::showMenu = false;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

    HANDLE hMutex = CreateMutexW(NULL, FALSE, L"MLP_External_Mutex");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        MessageBoxW(NULL, L"Application is already running!", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    util::OpenConsole();

    std::vector<std::thread> threads;

    bool menu_test = false;

    if (!menu_test) {
        game_window = FindWindow(NULL, L"LOCKDOWN Protocol  ");

        if (memory::attach("LOCKDOWN Protocol  ") != 0) {
            return 1;
        }

        memory::module_base = memory::get_module_base("LockdownProtocol-Win64-Shipping.exe", memory::process_id);
        if (!memory::module_base) {
            return 1;
        }

        threads.emplace_back(cache_useful);
        threads.emplace_back(CheckGameProcess);
        threads.emplace_back(item_data_cache);
        threads.emplace_back(ModulesThread);
        threads.emplace_back(SkinThread);
    }

    overlay = new c_overlay();
    overlay->bind_render_callback(render_callback);

    Settings::LoadHotkeys();
    Settings::LoadTeleports();

    Widgets::Init();
    Hotkeys::Init();

    threads.emplace_back(HotKeysThread);

    while (true) {
        overlay->msg_loop();
    }

    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }

    delete overlay;

    return 0;
}