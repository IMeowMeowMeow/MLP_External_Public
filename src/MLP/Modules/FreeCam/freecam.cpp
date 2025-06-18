#include "../../Settings/settings.h"
#include "../../Modules/modules.h"
#include "../../Headers/globals.h"
#include "../../Headers/util.hpp"
#include "../../Headers/KeyManager.h"
#include <iostream>
#include <thread>
#include <cmath>


std::atomic<bool> freecamRunning(false);
std::thread freecamThread;

void Modules::FreeCam::Enable() {
    if (freecamRunning) return;

    freecamRunning = true;

    DetachCamera();
    if (Settings::FlyHack::noPush) NoPush();
    freecamThread = std::thread(&Modules::FreeCam::Thread);
}

void Modules::FreeCam::Disable() {
    if (!freecamRunning) return;

    freecamRunning = false;

    AttachCamera();
    if (Settings::FlyHack::noPush) NoPush();

    if (freecamThread.joinable()) {
        freecamThread.join();
    }
}

bool Modules::FreeCam::IsRunning() {
    return freecamRunning;
}

void Modules::FreeCam::Thread() {
    while (freecamRunning) {
        local_camera_manager->get_last_cached_frame_private();

        float yaw = local_mec->get_yaw();
        float pitch = local_mec->get_pitch();

        auto camera = local_mec->get_camera();
        auto location = camera->get_location();

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

            camera->set_location(location);
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
            camera->set_location(location);
        }
        std::this_thread::sleep_for(std::chrono::microseconds(750));
    }
}






void Modules::FreeCam::DetachCamera() {
    memory::write(camera1_addr, FreeCamShellcode);
    memory::write(camera2_addr, FreeCamShellcode);
}

void Modules::FreeCam::AttachCamera() {
    memory::write(camera1_addr, Camera1Pattern);
    memory::write(camera2_addr, Camera2Pattern);
}

void Modules::FreeCam::NoPush() {
    if (Settings::FreeCam::enabled) memory::write(nopush_addr + 1, NoPushlShellcode);
    else memory::write(nopush_addr, NoPushPattern);
}


