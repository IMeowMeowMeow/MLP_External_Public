#pragma once

#ifndef ANIM_HPP
#define ANIM_HPP

#include <cmath>
#include <algorithm>

enum class AnimType
{
    Ease,
    EaseIn,
    EaseOut,
    EaseInOut,
    CubicBezier,
    Bounce,
    Elastic,
};

#define M_PI 3.14159265358979323846
namespace Anim {
    inline float Ease(float t) {
        return t * t * (3 - 2 * t);
    }

    inline float EaseIn(float t) {
        return t * t;
    }

    inline float EaseOut(float t) {
        return 1 - (1 - t) * (1 - t);
    }

    inline float EaseInOut(float t) {
        return t < 0.5 ? 2 * t * t : 1 - pow(-2 * t + 2, 2) / 2;
    }

    inline float CubicBezier(float t, float p0, float p1, float p2, float p3) {
        float u = 1 - t;
        float tt = t * t;
        float uu = u * u;
        float uuu = uu * u;
        float ttt = tt * t;
        return (uuu * p0) + (3 * uu * t * p1) + (3 * u * tt * p2) + (ttt * p3);
    }

    inline float Bounce(float t) {
        if (t < 1 / 2.75f) {
            return 7.5625f * t * t;
        }
        else if (t < 2 / 2.75f) {
            t -= 1.5f / 2.75f;
            return 7.5625f * t * t + 0.75f;
        }
        else if (t < 2.5f / 2.75f) {
            t -= 2.25f / 2.75f;
            return 7.5625f * t * t + 0.9375f;
        }
        else {
            t -= 2.625f / 2.75f;
            return 7.5625f * t * t + 0.984375f;
        }
    }

    inline float Elastic(float t) {
        return t == 0 ? 0 : t == 1 ? 1 : -pow(2, 10 * (t - 1)) * sin((t - 1.075) * (2 * M_PI) / 0.3);
    }


    inline float Animate(AnimType type, float t, float p0 = 0.0f, float p1 = 0.42f, float p2 = 0.58f, float p3 = 1.0f) {
        t = std::clamp(t, 0.0f, 1.0f);
        switch (type) {
        case AnimType::Ease: return Ease(t);
        case AnimType::EaseIn: return EaseIn(t);
        case AnimType::EaseOut: return EaseOut(t);
        case AnimType::EaseInOut: return EaseInOut(t);
        case AnimType::Bounce: return Bounce(t);
        case AnimType::Elastic: return Elastic(t);
        case AnimType::CubicBezier: return CubicBezier(t, p0, p1, p2, p3);
        default: return t;
        }
    }
}

#endif
