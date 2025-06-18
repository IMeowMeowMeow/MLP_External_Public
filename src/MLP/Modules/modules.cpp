#include "../Settings/Settings.h"
#include "../Headers/game_structures.hpp"
#include "Modules.h"
#include <iostream>
#include <thread>
#include <mutex>

void ManageModule(bool enabled, void(*EnableFunc)(), void(*DisableFunc)(), bool(*IsRunningFunc)()) {
    if (enabled && !IsRunningFunc()) {
        EnableFunc();
    }
    else if (!enabled && IsRunningFunc()) {
        DisableFunc();
    }
}


void Modules::Handler::Enable() {
    while (true) {
        ManageModule(Settings::Test::enabled, Modules::Test::Enable, Modules::Test::Disable, Modules::Test::IsRunning);
        ManageModule(Settings::Aimbot::enabled, Modules::Aimbot::Enable, Modules::Aimbot::Disable, Modules::Aimbot::IsRunning);
        ManageModule(Settings::FlyHack::enabled, Modules::FlyHack::Enable, Modules::FlyHack::Disable, Modules::FlyHack::IsRunning);
        ManageModule(Settings::FreeCam::enabled, Modules::FreeCam::Enable, Modules::FreeCam::Disable, Modules::FreeCam::IsRunning);
        ManageModule((Settings::Misc::infinityAmmo || Settings::Misc::noSpread ||
                      Settings::Misc::infinityStamina || Settings::Player::fov != 104 ||
                      Settings::Misc::godMode || Settings::Misc::infinityFlyStamina ||
                      Settings::Misc::freezeHandItem || Settings::Misc::freezeBagItem ||
                      Settings::Misc::speedHack || Settings::Player::runGun), Modules::Misc::Enable, Modules::Misc::Disable, Modules::Misc::IsRunning);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
