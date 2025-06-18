#include "../../Settings/settings.h"
#include "../../Modules/modules.h"
#include "../../Headers/globals.h"
#include "../../Headers/util.hpp"
#include "../../Headers/KeyManager.h"
#include <iostream>
#include <thread>
#include <cmath>


std::atomic<bool> flyhackRunning(false);
std::thread flyhackThread;
static bool wasAlive = false;

void Modules::FlyHack::Enable() {
    if (flyhackRunning) return;

    flyhackRunning = true;
    
    wasAlive = local_mec->get_alive();
    if(wasAlive) local_mec->set_alive(false);

    if (Settings::FlyHack::noFall) NoFall();
    if (Settings::FlyHack::noPush) NoPush();
    if (Settings::FlyHack::noClip) NoClip();

    flyhackThread = std::thread(&Modules::FlyHack::Thread);
}

void Modules::FlyHack::Disable() {
    if (!flyhackRunning) return;

    flyhackRunning = false;

    if(Settings::FlyHack::noFall) NoFall();
    if(Settings::FlyHack::noPush) NoPush();
    if(Settings::FlyHack::noClip) NoClip();
    
    if (flyhackThread.joinable()) {
        flyhackThread.join();
    }

    if (wasAlive) local_mec->set_alive(true);
    
    local_mec->set_health(9999);
}

bool Modules::FlyHack::IsRunning() {
    return flyhackRunning;
}

void Modules::FlyHack::Thread() {
    while (flyhackRunning) {
        float yaw = local_mec->get_yaw();
        float pitch = local_mec->get_pitch();

        vector3 location = local_mec->get_root_component()->get_relative_location();

        const float RAD = 3.14159265f / 180.0f;

        float RY = yaw * RAD;
        float RP = pitch * RAD;

        float SY = sin(RY);
        float CY = cos(RY);
        float SP = sin(RP);
        float CP = cos(RP);

        const double hSpeed = Settings::FlyHack::hSpeed;
        const double vSpeed = Settings::FlyHack::vSpeed;

        if (Settings::FlyHack::directional) {
            if (KeyManager::IsKeyPressed(Settings::FlyHack::forwardKey)) { // W
                location.x += CY * hSpeed; 
                location.y += SY * hSpeed;
                location.z += SP * vSpeed; 
            }
            if (KeyManager::IsKeyPressed(Settings::FlyHack::backwardKey)) { // S
                location.x -= CY * hSpeed; 
                location.y -= SY * hSpeed;
                location.z -= SP * vSpeed;  
            }
            if (KeyManager::IsKeyPressed(Settings::FlyHack::leftKey)) { // A
                location.x += SY * hSpeed;  
                location.y -= CY * hSpeed;
            }
            if (KeyManager::IsKeyPressed(Settings::FlyHack::rightKey)) { // D
                location.x -= SY * hSpeed; 
                location.y += CY * hSpeed;
            }
            if (KeyManager::IsKeyPressed(Settings::FlyHack::upKey)) { // SPACE
                location.z += vSpeed;  
            }
            if (KeyManager::IsKeyPressed(Settings::FlyHack::downKey)) { // CTRL 
                location.z -= vSpeed;  
            }

            local_mec->teleport2(location);
        }
        else {
            if (KeyManager::IsKeyPressed(Settings::FlyHack::forwardKey)) {  // W
                location.x += CY * hSpeed;
                location.y += SY * hSpeed;
            }
            if (KeyManager::IsKeyPressed(Settings::FlyHack::leftKey)) {  // A
                location.x += SY * hSpeed;
                location.y -= CY * hSpeed;
            }
            if (KeyManager::IsKeyPressed(Settings::FlyHack::backwardKey)) {  // S
                location.x -= CY * hSpeed;
                location.y -= SY * hSpeed;
            }
            if (KeyManager::IsKeyPressed(Settings::FlyHack::rightKey)) {  // D
                location.x -= SY * hSpeed;
                location.y += CY * hSpeed;
            }
            if (KeyManager::IsKeyPressed(Settings::FlyHack::upKey)) {  // SPACE
                location.z += vSpeed;
            }
            if (KeyManager::IsKeyPressed(Settings::FlyHack::downKey)) {  // CTRL
                location.z -= vSpeed;
            }
            local_mec->teleport2(location);
        }

        std::this_thread::sleep_for(std::chrono::microseconds(750));
    }
}

void Modules::FlyHack::NoFall() {
    if (Settings::FlyHack::enabled) memory::write(nofall_addr + 1, NoFallShellcode);
    else memory::write(nofall_addr, NoFallPattern);
}

void Modules::FlyHack::NoPush() {
    if (Settings::FlyHack::enabled) memory::write(nopush_addr + 1, NoPushlShellcode);
    else memory::write(nopush_addr, NoPushPattern);
}

void Modules::FlyHack::NoClip() {
    if (Settings::FlyHack::enabled) memory::write(noclip_addr, NoCliplShellcode);
    else memory::write(noclip_addr, NoClipPattern);
}


