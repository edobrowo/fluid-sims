#pragma once

#include <cmath>

#include "constants.hpp"
#include "util/common.hpp"

namespace math {

/// @brief Absolute value function.
template <Numeric T>
constexpr inline T abs(T value) {
    return (value >= T(0)) ? value : -value;
}

/// @brief Sign function.
template <Numeric T>
constexpr inline T sgn(T value) {
    return -T(value < T(0)) + T(value > T(0));
}

/// @brief Square function.
template <Numeric T>
constexpr inline T sqr(T value) {
    return value * value;
}

/// @brief Cube function.
template <Numeric T>
constexpr inline T cube(T value) {
    return value * value * value;
}

/// @brief Step function.
template <Numeric T>
constexpr inline T step(T value, T threshold) {
    return value < threshold ? T(0) : T(1);
}

/// @brief Clamp function.
template <Numeric T>
constexpr inline T clamp(T value, T low, T high) {
    if (value < low)
        return low;
    else if (value > high)
        return high;
    return value;
}

/// @brief Clamp function along [0, 1].
template <std::floating_point T>
constexpr inline T unit_clamp(T value) {
    return clamp(value, T(0), T(1));
}

/// @brief Linear interpolation.
template <std::floating_point T>
constexpr inline T lerp(T t, T a, T b) {
    return t * a + (T(1) - t) * b;
}

/// @brief Cubic interpolation.
template <std::floating_point T>
constexpr inline T cerp(T t, T a, T b, T c, T d) {
    const T t2 = t * t;
    const T t3 = t2 * t;

    // clang-format off
    return (       - T(0.5) * t +          t2 - T(0.5) * t3) * a +
           (T(1.0)              - T(2.5) * t2 + T(1.5) * t3) * b +
           (         T(0.5) * t + T(2.0) * t2 - T(1.5) * t3) * c +
           (                    - T(0.5) * t2 + T(0.5) * t3) * d;
    // clang-format on
}

/// @brief Scales a value with the range [min, max].
template <Numeric T>
constexpr inline T scale(T value, T min, T max) {
    assertm(max - min != T(0), "max - min is 0");
    return (value - min) / (max - min);
}

/// @brief Maps a value from range [min1, max1] to range [min2, max2].
template <Numeric T>
constexpr inline T map(T value, T min1, T max1, T min2, T max2) {
    assertm(max1 - min1 != T(0), "max1 - min1 is 0");
    return min2 + (value - min1) / (max1 - min1) * (max2 - min2);
}

/// @brief Hermite function.
template <Numeric T>
constexpr inline T hermite(T value) {
    return value * value * (T(3) - T(2) * value);
}

/// @brief Smoothstep function.
template <Numeric T>
constexpr inline T smoothstep(T left, T right, T t) {
    return hermite(unit_clamp(scale(t, left, right)));
}

/// @brief Integer divides `num` by `denom`, rounding up.
template <std::integral T>
constexpr inline T divceil(T num, T denom) {
    return num / denom + (num % denom != T(0));
}

/// @brief Converts degrees to radians.
template <std::floating_point T>
constexpr inline T rad(T deg) {
    return deg * pi<T>() / T(180.0);
}

/// @brief Converts radians to degrees.
template <std::floating_point T>
constexpr inline T deg(T rad) {
    return rad * T(180.0) / pi<T>();
}

}
