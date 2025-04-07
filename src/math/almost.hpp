#pragma once

#include "common.hpp"

namespace almost {

/// @brief Absolute error for approximate float comparisons.
constexpr f64 EPSILON = 0.00001;

/// @brief Almost equal to zero.
template <std::floating_point T>
constexpr inline bool eq_zero(T a) {
    return math::abs(a) <= T(EPSILON);
}

/// @brief Almost not equal to zero.
template <std::floating_point T>
constexpr inline bool ne_zero(T a) {
    return math::abs(a) > T(EPSILON);
}

/// @brief Almost greater than zero.
template <std::floating_point T>
constexpr inline bool g_zero(T a) {
    return a > T(EPSILON);
}

/// @brief Almost greater than or equal to zero.
template <std::floating_point T>
constexpr inline bool ge_zero(T a) {
    return a >= -T(EPSILON);
}

/// @brief Almost less than zero.
template <std::floating_point T>
constexpr inline bool l_zero(T a) {
    return a < -T(EPSILON);
}

/// @brief Almost less than or equal to zero.
template <std::floating_point T>
constexpr inline bool le_zero(T a) {
    return a <= T(EPSILON);
}

/// @brief Almost equal.
template <std::floating_point T>
constexpr inline bool eq(T a, T b) {
    return math::abs(a - b) <= T(EPSILON);
}

/// @brief Almost not equal.
template <std::floating_point T>
constexpr inline bool ne(T a, T b) {
    return math::abs(a - b) > T(EPSILON);
}

/// @brief Almost greater than.
template <std::floating_point T>
constexpr inline bool g(T a, T b) {
    return (a - b) > T(EPSILON);
}

/// @brief Almost greater than or equal to.
template <std::floating_point T>
constexpr inline bool ge(T a, T b) {
    return (a - b) >= -T(EPSILON);
}

/// @brief Almost less than.
template <std::floating_point T>
constexpr inline bool l(T a, T b) {
    return (a - b) < -T(EPSILON);
}

/// @brief Almost less than or equal to.
template <std::floating_point T>
constexpr inline bool le(T a, T b) {
    return (a - b) <= T(EPSILON);
}

}
