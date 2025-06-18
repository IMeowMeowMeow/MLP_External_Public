#pragma once
#include <string>
#include <functional>
#include "game_math.hpp"
#include "enums.h"

struct Teleport {
    std::string name;
    vector3 location;
    vector2 angle;
    int hotkey;

    bool operator==(const Teleport& other) const {
        return name == other.name &&
            location == other.location &&
            angle == other.angle &&
            hotkey == other.hotkey;
    }
};

namespace std {
    template<>
    struct hash<Teleport> {
        std::size_t operator()(const Teleport& teleport) const {
            std::size_t h1 = std::hash<std::string>()(teleport.name);
            std::size_t h2 = std::hash<double>()(teleport.location.x);
            std::size_t h3 = std::hash<double>()(teleport.location.y);
            std::size_t h4 = std::hash<double>()(teleport.location.z);
            std::size_t h5 = std::hash<double>()(teleport.angle.x);
            std::size_t h6 = std::hash<double>()(teleport.angle.y);
            std::size_t h7 = std::hash<int>()(teleport.hotkey);
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4) ^ (h6 << 5) ^ (h7 << 6);
        }
    };
}

struct SnapSpot {
    SpotGroup group;
    int index;

    bool operator==(const SnapSpot& other) const {
        return group == other.group && index == other.index;
    }
};



namespace std {
    template<>
    struct hash<SnapSpot> {
        std::size_t operator()(const SnapSpot& spot) const {
            std::size_t h1 = std::hash<SpotGroup>()(spot.group);
            std::size_t h2 = std::hash<int>()(spot.index);

            return h1 ^ (h2 << 1);
        }
    };
}