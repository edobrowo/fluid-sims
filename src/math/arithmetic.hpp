#pragma once

#include <cmath>

#include "util/common.hpp"

namespace math {

template <std::floating_point T>
constexpr inline T pi() {
    return T(3.141592653589793115997963468544185161590576171875);
}

template <std::floating_point T>
constexpr inline T half_pi() {
    return T(1.57079632679489655799898173427209258079528808593755);
}

template <std::floating_point T>
constexpr inline T quarter_pi() {
    return T(0.78539816339744827899949086713604629039764404296875);
}

template <std::floating_point T>
constexpr inline T inv_pi() {
    return T(0.318309886183790691216444201927515678107738494873046875);
}

template <std::floating_point T>
constexpr inline T tau() {
    return T(6.28318530717958623199592693708837032318115234375);
}

template <std::floating_point T>
constexpr inline T inv_tau() {
    return T(0.1591549430918953456082221009637578390538692474365234375);
}

template <std::floating_point T>
constexpr inline T e() {
    return T(2.718281828459045090795598298427648842334747314453125);
}

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
    return value < threshold ? T(0) : T(1);
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
    return clamp(value, T(0), T(1));
}

template <std::floating_point T>
constexpr inline T lerp(T t, T a, T b) {
    return t * a + (T(1) - t) * b;
}

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

template <Numeric T>
constexpr inline T scale(T value, T min, T max) {
    assertm(max - min != T(0), "max - min is 0");
    return (value - min) / (max - min);
}

template <Numeric T>
constexpr inline T map(T value, T min1, T max1, T min2, T max2) {
    assertm(max1 - min1 != T(0), "max1 - min1 is 0");
    return min2 + (value - min1) / (max1 - min1) * (max2 - min2);
}

template <Numeric T>
constexpr inline T hermite(T value) {
    return value * value * (T(3) - T(2) * value);
}

template <Numeric T>
constexpr inline T smoothstep(T left, T right, T t) {
    return hermite(unit_clamp(scale(t, left, right)));
}

template <std::integral T>
constexpr inline T divceil(T num, T denom) {
    return num / denom + (num % denom != T(0));
}

template <std::floating_point T>
constexpr inline T rad(T deg) {
    return deg * pi<T>() / T(180.0);
}

template <std::floating_point T>
constexpr inline T deg(T rad) {
    return rad * T(180.0) / pi<T>();
}

}
