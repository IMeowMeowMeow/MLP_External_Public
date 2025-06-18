#pragma once
#include <string>
#include "game_math.hpp"
#include <iostream>
#include <iomanip>

struct BOT {
    std::string name;
    int role;
    vector3 position;
    vector3 ghost_position;
};

static BOT Aleksey = { "BOT Aleksey", 3, vector3(9500.0, 8500.0, 0.0), vector3(9500.0, 8500.0, 150.0) };
static BOT Vladimir = { "BOT Vladmir", 4, vector3(9500.0, 9000.0, 0.0), vector3(9500.0, 9000.0, 150.0) };
static BOT Nikita = { "BOT Nikita", 3, vector3(9000.0, 10000.0, 0.0), vector3(8500.0, 10000.0, 150.0) };

inline std::vector<BOT> bot_list{
    Aleksey,
    Vladimir,
    Nikita
};

