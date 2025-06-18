#pragma once

#include <functional>
#include "../MLP/Headers/enums.h"

struct Hotkey {
    int key;
    HotkeyMode mode;
    WidgetType type;
    std::function<void()> onActivate;
    std::function<void()> onDeactivate;
    bool justAssigned;

    Hotkey(int key = 0, HotkeyMode mode = HotkeyMode::Toggle, WidgetType type = WidgetType::None,
        std::function<void()> onActivate = nullptr,
        std::function<void()> onDeactivate = nullptr,
        bool justAssigned = false)
        : key(key), mode(mode), type(type), onActivate(onActivate), onDeactivate(onDeactivate), justAssigned(justAssigned) {
    };
};

namespace Hotkeys {
    void Init();
    void AssignHotkey(const std::string& label, Hotkey& hotkey);
    std::function<void()> GetSpecialActions(const std::string& label, const WidgetType& type, bool* boolPtr);
    inline std::unordered_map<std::string, Hotkey> HotkeysMap;
}