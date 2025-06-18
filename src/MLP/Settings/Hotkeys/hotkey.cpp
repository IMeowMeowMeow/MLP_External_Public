#include "../settings.h"
#include "hotkey.h"
#include "../../Headers/widget.hpp"
#include "../../Modules/modules.h"

void Hotkeys::Init() {
    for (auto& [label, hotkey] : HotkeysMap) {
        AssignHotkey(label, hotkey);
    }
}

void Hotkeys::AssignHotkey(const std::string& label, Hotkey& hotkey) {
    if (hotkey.type == WidgetType::Checkbox) {
        auto& checkbox = Widgets::Checkboxes[label];

        if (auto boolPtr = std::get_if<bool*>(&checkbox.value)) {
            if (hotkey.mode == HotkeyMode::Toggle) {
                auto specialAction = GetSpecialActions(label, hotkey.type, *boolPtr);

                if (*boolPtr) { 
                    if (specialAction) {
                        bool* settingPtr = *boolPtr; 
                        hotkey.onActivate = [settingPtr, specialAction] { *settingPtr = !*settingPtr;  specialAction(); };
                    }
                    else {
                        bool* settingPtr = *boolPtr; 
                        hotkey.onActivate = [settingPtr] { *settingPtr = !*settingPtr; };
                    }
                }
            }
            else if (hotkey.mode == HotkeyMode::Hold) {
                auto specialAction = GetSpecialActions(label, hotkey.type, *boolPtr);

                if (*boolPtr) {
                    if (specialAction) {
                        bool* settingPtr = *boolPtr;
                        hotkey.onActivate = [settingPtr, specialAction] { *settingPtr = !*settingPtr;  specialAction(); };
                        hotkey.onDeactivate = [settingPtr, specialAction] { *settingPtr = !*settingPtr;  specialAction(); };
                    }
                    else {
                        bool* settingPtr = *boolPtr;
                        hotkey.onActivate = [settingPtr] { *settingPtr = !*settingPtr; };
                        hotkey.onDeactivate = [settingPtr] { *settingPtr = !*settingPtr; };
                    }
                }
            }
        }
    }
}

std::function<void()> Hotkeys::GetSpecialActions(const std::string& label, const WidgetType& type, bool* boolPtr) {
    if (!boolPtr) return nullptr;

    if (type == WidgetType::Checkbox) {
        if (label == "Alive") {
            return [boolPtr] { Modules::Misc::SetAlive(*boolPtr); };
        }
        else if (label == "Silent Gun") {
            return [boolPtr] { Modules::Misc::SetSilentGun(*boolPtr); };
        }
    }
    return nullptr;
}