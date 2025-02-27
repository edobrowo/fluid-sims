#pragma once

#include <cmath>

#include "util/common.hpp"

namespace math {

// clang-format off

constexpr f64 Pi     = 3.14159265358979323846;
constexpr f64 PiH    = 1.57079632679489661923;
constexpr f64 PiQ    = 0.78539816339744830961;
constexpr f64 InvPi  = 0.31830988618379067154;
constexpr f64 Tau    = 6.28318530717958647692;
constexpr f64 InvTau = 0.15915494309189533577;

// clang-format on

template <Numeric T>
constexpr inline T sqr(T value) {
    return value * value;
}

template <Numeric T>
constexpr inline T cube(T value) {
    return value * value * value;
}

template <Numeric T>
constexpr inline T step(T value, T threshold) {
    return value < threshold ? 0.0 : 1.0;
}

template <Numeric T>
constexpr inline T clamp(T value, T low, T high) {
    if (value < low)
        return low;
    else if (value > high)
        return high;
    return value;
}

template <std::floating_point T>
constexpr inline T unit_clamp(T value) {
    return clamp(value, 0.0, 1.0);
}

template <std::floating_point T>
constexpr inline T lerp(T t, T a, T b) {
    return t * a + (1.0 - t) * b;
}

template <Numeric T>
constexpr inline T scale(T value, T min, T max) {
    assertm(max - min != 0, "max - min is 0");
    return (value - min) / (max - min);
}

template <Numeric T>
constexpr inline T map(T value, T min1, T max1, T min2, T max2) {
    assertm(max1 - min1 != 0, "max1 - min1 is 0");
    return min2 + (value - min1) / (max1 - min1) * (max2 - min2);
}

template <Numeric T>
constexpr inline T hermite(T value) {
    return value * value * (3.0 - 2.0 * value);
}

template <Numeric T>
constexpr inline T smoothstep(T left, T right, T t) {
    return hermite(unit_clamp(scale(t, left, right)));
}

template <std::integral T>
constexpr inline T divceil(T num, T denom) {
    return num / denom + (num % denom != 0);
}

template <std::floating_point T>
constexpr inline T rad(T deg) {
    return deg * Pi / static_cast<T>(180.0);
}

template <std::floating_point T>
constexpr inline T deg(T rad) {
    return rad * static_cast<T>(180.0) / Pi;
}

}
