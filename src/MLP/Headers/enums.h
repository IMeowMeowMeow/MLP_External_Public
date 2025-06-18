#pragma once

#ifndef ENUMS_H
#define ENUMS_H

enum class InventorySlot {
    Hand,
    Bag
};

enum class Tab {
    Aimbot,
    Visuals,
    Inventory,
    World,
    Player,
    Movement,
    Teleport,
    Misc,
    Settings
};

enum class SubTabs {
    Menu,
    Config,
    Hotkeys,
    Players,
    Objects,
    Visuals,
    Colors,
};

enum class ItemGroup {
    Unknown,
    Weapons,
    Primary,
    Secondary
};

enum class Access {
    Denied,
    Approved,
};

enum class Reason {
    Timeout,
    NoAuthCode,
    NoAccessToken,
    NoUserID,
    NotOnServer,
    NoRole,
    RoleRemoved,
    NoReason,
};

enum class AimTarget {
    Head,
    Neck,
    Body
};

enum class HotkeyMode {
    Hold,
    Toggle,
    Press,
    None
};

enum class WidgetType {
    Checkbox,
    Slider,
    Button,
    None
};

enum class SpotGroup {
    Top,
    Bottom,
    Left,
    Right
};

enum class E_PlayerRole {
    NotReady = 1,
    Ready = 2,
    Employee = 3,
    Dissident = 4,
    Dead = 5,
};

enum class E_SessionState {
    Solo = 0,
    Host = 1,
    Client = 2,
};

enum class E_MoveInputState
{
    Null = 0,
    Forward = 1,
    Backward = 2,
    Right = 3,
    Left = 4,
};

enum class E_Stance {
    Stand = 0,
    Walk = 1,
    Jog = 2,
    Air = 3,
    Sit = 4,
    Run = 5,
};

enum class E_Logic {
    None = 0,
    Container = 1,
    Sample = 2,
    Seringe = 3,
    Fuze = 4,
    Package = 5,
    GazBottle = 6,
    Filter = 7,
    Rice = 8,
    Fish = 9,
    Pizzushi = 10,
    C4 = 11,
    Detonator = 12,
    Zapette = 13,
};

enum class  E_ShotPattern {
    Classic = 0,
    Shorty = 1,
    Shotgun = 2,
};

enum class  E_ConsumableType {
    None = 0,
    Seringe = 1,
    Bottle = 2,
    Grenade = 3,
};

enum class  E_ItemType {
    None = 0,
    Heal = 1,
    Strength = 2,
    Armor = 3,
    Sneak = 4,
    Debuff = 5,
    Buff1 = 6,
    Buff2 = 7,
    TaskData = 8,
    UpdateData = 9,
    VirusData = 10,
    Rice = 11,
};

enum class  E_ItemPlaceType {
    None = 0,
    Drop = 1,
    Place = 2,
    Spawn = 3,
};

static const char* E_Logic_Names[] = {
    "None",
    "Container",
    "Sample",
    "Seringe",
    "Fuze",
    "Package",
    "GazBottle",
    "Filter",
    "Rice",
    "Fish",
    "Pizzushi",
    "C4",
    "Detonator",
    "Zapette"
};

#endif