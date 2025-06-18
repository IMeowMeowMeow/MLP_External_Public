#pragma once

#ifndef KEYMANAGER_H
#define KEYMANAGER_H

#include <unordered_map>
#include <Windows.h>

class KeyManager {
public:
    static bool IsKeyPressed(int key) {
        return (GetAsyncKeyState(key) & 0x8000) != 0;
    }

    static bool IsKeyToggled(int key) {
        if (IsKeyPressed(key)) {
            if (!toggleKeyStates[key]) {
                toggleKeyStates[key] = true;
                return true;
            }
        }
        else {
            toggleKeyStates[key] = false;
        }
        return false;
    }

    static bool IsKeyReleased(int key) {
        bool isActive = IsKeyPressed(key);
        bool& wasHeld = heldKeyStates[key];

        if (!isActive && wasHeld) {
            wasHeld = false;
            return true;
        }

        if (isActive) {
            wasHeld = true;
        }

        return false;
    }

private:
    static inline std::unordered_map<int, bool> toggleKeyStates;
    static inline std::unordered_map<int, bool> heldKeyStates;
};

#endif