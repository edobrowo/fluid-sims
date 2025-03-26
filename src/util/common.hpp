#pragma once

#include <concepts>
#include <cstdint>
#include <iostream>
#include <type_traits>

/// @brief 8-bit signed integral type.
using i8 = std::int8_t;
/// @brief 16-bit signed integral type.
using i16 = std::int16_t;
/// @brief 32-bit signed integral type.
using i32 = std::int32_t;
/// @brief 64-bit signed integral type.
using i64 = std::int64_t;

/// @brief 8-bit unsigned integral type.
using u8 = std::uint8_t;
/// @brief 16-bit unsigned integral type.
using u16 = std::uint16_t;
/// @brief 32-bit unsigned integral type.
using u32 = std::uint32_t;
/// @brief 64-bit unsigned integral type.
using u64 = std::uint64_t;

/// @brief 32-bit floating-point real type.
using f32 = float;
/// @brief 64-bit floating-point real type.
using f64 = double;

/// @brief Container size.
using Size = std::uint64_t;

/// @brief Index into a contiguous container.
using Index = std::uint64_t;

/// @brief Integral or floating-point type.
template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

/// @brief Signed integral type.
template <typename T>
concept SignedIntegral = std::integral<T> && std::is_signed_v<T>;

/// @brief Unsigned integral type.
template <typename T>
concept UnsignedIntegral = std::integral<T> && std::is_unsigned_v<T>;

// clang-format off

#define TO_STR_INNER(X) #X
#define TO_STR(X) TO_STR_INNER(X)

#define SOURCE_LOC               \
    __FILE__                     \
    " : " TO_STR(__LINE__)

/// @brief Basic assertion with a log message.
#define assertm(X, M)            \
    if (!(X)) {                  \
        std::cout <<             \
            "assertion failed: " \
            #X                   \
            "\n"                 \
            #M                   \
            "\n"                 \
            SOURCE_LOC           \
            "\n";                \
        std::exit(1);            \
    }

/// @brief Unimplemented code.
#define unimplemented std::cout <<          \
    "control reached unimplemented code:\n" \
    SOURCE_LOC                              \
    "\n";                                   \
    std::exit(1);

/// @brief Indicates that this path of control flow should not be compiled.
/// @cite https://en.cppreference.com/w/cpp/utility/unreachable
[[noreturn]] static inline void unreachable_internal() {
#if defined(_MSC_VER) && !defined(__clang__)  // MSVC
    __assume(false);
#else  // GCC, Clang
    __builtin_unreachable();
#endif
}

/// @brief Indicates that this path of control flow should not be compiled.
#define unreachable unreachable_internal();

// clang-format on
