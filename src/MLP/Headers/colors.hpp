#pragma once

#ifndef COLORS_HPP
#define COLORS_HPP

#include "util.hpp"

namespace Colors {

    inline ImColor White(255, 255, 255, 255);
    inline ImColor Red(255, 0, 0, 255);
    inline ImColor Green(0, 255, 0, 255);
    inline ImColor Blue(0, 0, 255, 255);
    inline ImColor Yellow(255, 255, 0, 255);
    inline ImColor Orange(255, 165, 0, 255);
    inline ImColor Purple(128, 0, 128, 255);

    inline ImColor Main(4, 4, 4, 255);
    inline ImColor Line(20, 20, 22, 255);

    inline ImColor Window(18, 18, 20, 220);
    inline ImColor SideBar(18, 18, 20, 255);
    inline ImColor WindowHeader(20, 20, 22, 220);

    inline ImColor TextDisabled(150, 150, 150, 245);
    inline ImColor TextEnabled(245, 245, 245, 255);
    inline ImColor TextHovered(220, 220, 220, 255);
    inline ImColor TextActive(235, 235, 235, 255);

    inline ImColor Brightness(const ImColor& color, float percentage)
    {
        int r = color.Value.x * 255;
        int g = color.Value.y * 255;
        int b = color.Value.z * 255;
        int a = color.Value.w * 255;

        r = util::Min(255, static_cast<int>(r * (1.0f + percentage)));
        g = util::Min(255, static_cast<int>(g * (1.0f + percentage)));
        b = util::Min(255, static_cast<int>(b * (1.0f + percentage)));

        return ImColor(r, g, b, a);
    }

    inline ImColor Mute(const ImColor& color, float percentage)
    {
        int r = color.Value.x * 255;
        int g = color.Value.y * 255;
        int b = color.Value.z * 255;
        int a = color.Value.w * 255;

        r = util::Max(0, static_cast<int>(r * (1.0f - percentage)));
        g = util::Max(0, static_cast<int>(g * (1.0f - percentage)));
        b = util::Max(0, static_cast<int>(b * (1.0f - percentage)));

        return ImColor(r, g, b, a);
    }
}

#endif