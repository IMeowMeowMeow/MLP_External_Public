#pragma once

#include <Windows.h>  
#include <WinUser.h>

inline static const int key_codes[] = {
    0x0,          // OFF
    VK_LBUTTON,   // LMB
    VK_RBUTTON,   // RMB
    VK_CANCEL,    // Cancel
    VK_MBUTTON,   // MMB
    VK_XBUTTON1,  // M3
    VK_XBUTTON2,  // M4
    0x07,         // Unknown
    VK_BACK,      // Back
    VK_TAB,       // Tab
    0x0A,         // Unknown
    0x0B,         // Unknown
    VK_CLEAR,     // Clear
    VK_RETURN,    // Enter
    0x0E,         // Unknown
    0x0F,         // Unknown
    VK_LSHIFT,    // LShift
    VK_RSHIFT,    // RShift
    VK_LCONTROL,  // LControl
    VK_RCONTROL,  // RControl
    VK_LMENU,     // LAlt
    VK_RMENU,     // RAlt
    VK_PAUSE,     // Pause
    VK_CAPITAL,   // CapsLock
    VK_ESCAPE,    // Escape
    VK_SPACE,     // Space
    VK_PRIOR,     // Page Up
    VK_NEXT,      // Page Down
    VK_END,       // End
    VK_HOME,      // Home
    VK_LEFT,      // Left Arrow
    VK_UP,        // Up Arrow
    VK_RIGHT,     // Right Arrow
    VK_DOWN,      // Down Arrow
    VK_SELECT,    // Select
    VK_PRINT,     // Print
    VK_EXECUTE,   // Execute
    VK_SNAPSHOT,  // Print Screen
    VK_INSERT,    // Insert
    VK_DELETE,    // Delete
    VK_HELP,      // Help
    0x30,         // 0
    0x31,         // 1
    0x32,         // 2
    0x33,         // 3
    0x34,         // 4
    0x35,         // 5
    0x36,         // 6
    0x37,         // 7
    0x38,         // 8
    0x39,         // 9
    0x41,         // A
    0x42,         // B
    0x43,         // C
    0x44,         // D
    0x45,         // E
    0x46,         // F
    0x47,         // G
    0x48,         // H
    0x49,         // I
    0x4A,         // J
    0x4B,         // K
    0x4C,         // L
    0x4D,         // M
    0x4E,         // N
    0x4F,         // O
    0x50,         // P
    0x51,         // Q
    0x52,         // R
    0x53,         // S
    0x54,         // T
    0x55,         // U
    0x56,         // V
    0x57,         // W
    0x58,         // X
    0x59,         // Y
    0x5A,         // Z
    VK_LWIN,      // LWin
    VK_RWIN,      // RWin
    VK_APPS,      // Apps
    VK_SLEEP,     // Sleep
    VK_NUMPAD0,   // Numpad 0
    VK_NUMPAD1,   // Numpad 1
    VK_NUMPAD2,   // Numpad 2
    VK_NUMPAD3,   // Numpad 3
    VK_NUMPAD4,   // Numpad 4
    VK_NUMPAD5,   // Numpad 5
    VK_NUMPAD6,   // Numpad 6
    VK_NUMPAD7,   // Numpad 7
    VK_NUMPAD8,   // Numpad 8
    VK_NUMPAD9,   // Numpad 9
    VK_MULTIPLY,  // Multiply
    VK_ADD,       // Add
    VK_SEPARATOR, // Separator
    VK_SUBTRACT,  // Subtract
    VK_DECIMAL,   // Decimal
    VK_DIVIDE,    // Divide
    VK_F1,        // F1
    VK_F2,        // F2
    VK_F3,        // F3
    VK_F4,        // F4
    VK_F5,        // F5
    VK_F6,        // F6
    VK_F7,        // F7
    VK_F8,        // F8
    VK_F9,        // F9
    VK_F10,       // F10
    VK_F11,       // F11
    VK_F12,       // F12
    VK_F13,       // F13
    VK_F14,       // F14
    VK_F15,       // F15
    VK_F16,       // F16
    VK_F17,       // F17
    VK_F18,       // F18
    VK_F19,       // F19
    VK_F20,       // F20
    VK_F21,       // F21
    VK_F22,       // F22
    VK_F23,       // F23
    VK_F24,       // F24
    VK_NUMLOCK,   // NumLock
    VK_SCROLL,    // ScrollLock
    VK_OEM_NEC_EQUAL, // OEM_NEC_EQUAL
    VK_OEM_FJ_MASSHOU,  // OEM_FJ_MASSHOU
    VK_OEM_FJ_TOUROKU,  // OEM_FJ_TOUROKU
    VK_OEM_FJ_LOYA,      // OEM_FJ_LOYA
    VK_OEM_FJ_ROYA,      // OEM_FJ_ROYA
    VK_OEM_1,       // ;
    0xBF,           // <
    VK_OEM_3,       // >
    VK_OEM_4,       // /
    VK_OEM_5,       // [
    VK_OEM_6,
    VK_OEM_7,       // ]
    VK_OEM_8        // '
};

inline static const char* key_names[] = {
    "Unbound",
    "LMB",
    "RMB",
    "Cancel",
    "MMB",
    "M3",
    "M4",
    "Unknown",
    "Back",
    "Tab",
    "Unknown",
    "Unknown",
    "Clear",
    "Enter",
    "Unknown",
    "Unknown",
    "Shift",
    "Control",
    "LALT",
    "Pause",
    "Caps",
    "VK_KANA",
    "Unknown",
    "VK_JUNJA",
    "VK_FINAL",
    "VK_KANJI",
    "Unknown",
    "Esc",
    "VK_CONVERT",
    "VK_NONCONVERT",
    "VK_ACCEPT",
    "VK_MODECHANGE",
    "Space",
    "Prior",
    "Next",
    "End",
    "Home",
    "Left",
    "Up",
    "Right",
    "Down",
    "Select",
    "Print",
    "Execute",
    "Print",
    "Insert",
    "Delete",
    "Help",
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "VK_LWIN",
    "VK_RWIN",
    "VK_APPS",
    "Unknown",
    "VK_SLEEP",
    "Num 0",
    "Num 1",
    "Num 2",
    "Num 3",
    "Num 4",
    "Num 5",
    "Num 6",
    "Num 7",
    "Num 8",
    "Num 9",
    "Multiply",
    "Add",
    "Seperator",
    "Subtract",
    "Decimal",
    "Divide",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
    "F13",
    "F14",
    "F15",
    "F16",
    "F17",
    "F18",
    "F19",
    "F20",
    "F21",
    "F22",
    "F23",
    "F24",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Numlock",
    "Scroll",
    "OEM_NEC_EQUAL",
    "OEM_FJ_MASSHOU",
    "OEM_FJ_TOUROKU",
    "OEM_FJ_LOYA",
    "OEM_FJ_ROYA",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "LShift",
    "RShift",
    "LCTRL",
    "RCTRL",
    "LALT",
    "RALT",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    ";",
    "+",
    "<",
    "-",
    ">",
    "/",
    "~",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "[",
    "\\",
    "]",
    "'"
};