#pragma once

#ifndef WIDGET_HPP
#define WIDGET_HPP

#include "enums.h"
#include <string>
#include <variant>
#include <vector>
#include <thread>
#include <unordered_map>
#include "../Settings/settings.h"

struct Widget {
    Widget() = default;

    WidgetType type;
    std::variant<bool*, float*, double*, int*> value;


    virtual ~Widget() = default;
};

struct Checkbox : public Widget {
    Checkbox() = default;

    Checkbox(bool* value) {
        this->type = WidgetType::Checkbox;
        this->value = value;
    }
};

struct SliderFloat : public Widget {
    SliderFloat() = default;  

    SliderFloat(float* value, float min, float max, const std::string& format = "%.2lf") {
        this->type = WidgetType::Slider;
        this->value = value;
        this->min = min;
        this->max = max;
        this->format = format;
    }

    float min;
    float max;
    std::string format;
};

struct SliderDouble : public Widget {
    SliderDouble() = default;

    SliderDouble(double* value, double min, double max, const std::string& format = "%.2lf") {
        this->type = WidgetType::Slider;
        this->value = value;
        this->min = min;
        this->max = max;
        this->format = format;
    }

    double min;
    double max;
    std::string format;
};

struct SliderInt : public Widget {
    SliderInt() = default;

    SliderInt(int* value, int min, int max, const std::string& format = "%d") {
        this->type = WidgetType::Slider;
        this->value = value;
        this->min = min;
        this->max = max;
        this->format = format;
    }

    int min;         
    int max;
    std::string format;
};


namespace Widgets {
    inline void Init();

    inline std::unordered_map<std::string, Checkbox> Checkboxes {};
    inline std::unordered_map<std::string, SliderFloat> FloatSliders {};
    inline std::unordered_map<std::string, SliderDouble> DoubleSliders {};
    inline std::unordered_map<std::string, SliderInt> IntSliders {};
}

void Widgets::Init() {
    Checkboxes.emplace("Enable Aimbot", Checkbox(&Settings::Aimbot::enabled));
    Checkboxes.emplace("CheckFOV", Checkbox(&Settings::Aimbot::checkFOV));
    Checkboxes.emplace("DrawFOV", Checkbox(&Settings::Aimbot::drawFOV));

    FloatSliders.emplace("Smooth", SliderFloat(&Settings::Aimbot::smooth, 0.0f, 0.99f, "%.2f"));
    FloatSliders.emplace("FOV##aimbot", SliderFloat(&Settings::Aimbot::fov, 1.0f, 60.0f, "%.1f"));
    IntSliders.emplace("Trigger Shoot Delay", SliderInt(&Settings::Aimbot::triggerShootDelay, 0, 500));

    Checkboxes.emplace("Enable Players ESP", Checkbox(&Settings::ESP::Players::enabled));
    Checkboxes.emplace("Player", Checkbox(&Settings::ESP::Players::player));
    Checkboxes.emplace("Ghost", Checkbox(&Settings::ESP::Players::ghost));
    Checkboxes.emplace("Dead Body", Checkbox(&Settings::ESP::Players::deadBody));
    Checkboxes.emplace("Skeleton", Checkbox(&Settings::ESP::Players::skeleton));
    Checkboxes.emplace("Dissidents", Checkbox(&Settings::ESP::Players::dissidents));
    Checkboxes.emplace("2D Box##players", Checkbox(&Settings::ESP::Players::box));
    Checkboxes.emplace("Name##players", Checkbox(&Settings::ESP::Players::name));
    Checkboxes.emplace("Distance##players", Checkbox(&Settings::ESP::Players::distance));
    Checkboxes.emplace("Snapline##players", Checkbox(&Settings::ESP::Players::snapline));
    Checkboxes.emplace("Inventory##players", Checkbox(&Settings::ESP::Players::inventory));

    DoubleSliders.emplace("Max Distance##players", SliderDouble(&Settings::ESP::Players::maxDistance, 0.0, 150.0, "%.0lf"));

    Checkboxes.emplace("Enable Items ESP", Checkbox(&Settings::ESP::Items::enabled));
    Checkboxes.emplace("Name##Items", Checkbox(&Settings::ESP::Items::name));
    Checkboxes.emplace("State##Items", Checkbox(&Settings::ESP::Items::state));
    Checkboxes.emplace("Distance##Items", Checkbox(&Settings::ESP::Items::distance));

    DoubleSliders.emplace("Max Distance##Items", SliderDouble(&Settings::ESP::Items::maxDistance, 0.0, 150.0, "%.0lf"));

    FloatSliders.emplace("FOV##player", SliderFloat(&Settings::Player::fov, 1.0f, 179.9f, "%.2f"));
    Checkboxes.emplace("Alive", Checkbox(&Settings::Player::alive));
    Checkboxes.emplace("Infinity Stamina", Checkbox(&Settings::Misc::infinityStamina));
    Checkboxes.emplace("Infinity Fly Stamina", Checkbox(&Settings::Misc::infinityFlyStamina));
    Checkboxes.emplace("Run Gun", Checkbox(&Settings::Player::runGun));
    Checkboxes.emplace("Silent Gun", Checkbox(&Settings::Misc::silentGun));
    Checkboxes.emplace("No Spread", Checkbox(&Settings::Misc::noSpread));
    Checkboxes.emplace("Rainbow Skin", Checkbox(&Settings::Misc::rainbowSkin));
    Checkboxes.emplace("Infinity Ammo", Checkbox(&Settings::Misc::infinityAmmo));

    Checkboxes.emplace("Enable FlyHack", Checkbox(&Settings::FlyHack::enabled));
    Checkboxes.emplace("Enable FreeCam", Checkbox(&Settings::FreeCam::enabled));
    //Checkboxes.emplace("NoPush", Checkbox(&Settings::Misc::noPush));
    //Checkboxes.emplace("NoFall", Checkbox(&Settings::Misc::noFall));

    FloatSliders.emplace("Horizontal", SliderFloat(&Settings::FlyHack::hSpeed, 0.01f, 500.0f, "%.2f"));
    FloatSliders.emplace("Vertical", SliderFloat(&Settings::FlyHack::vSpeed, 0.01f, 500.0f, "%.2f"));
    Checkboxes.emplace("Directional", Checkbox(&Settings::FlyHack::directional));

    Checkboxes.emplace("Rainbow Color", Checkbox(&Settings::Menu::rainbowColor));

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

#endif