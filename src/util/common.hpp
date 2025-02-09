#pragma once

#include <concepts>
#include <cstdint>
#include <iostream>
#include <type_traits>

/// Fixed-size signed integral types.
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

/// Fixed-size unsigned integral types.
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

/// Fixed-size float types.
using f32 = float;
using f64 = double;

/// Indicates the memory size of a container.
using Size = uint64_t;

/// Indicates a type-based index into a container.
using Index = uint64_t;

/// Any integral or floating numeric type.
template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

/// Any signed integral type.
template <typename T>
concept SignedIntegral = std::integral<T> && std::is_signed_v<T>;

/// Any unsigned integral type.
template <typename T>
concept UnsignedIntegral = std::integral<T> && std::is_unsigned_v<T>;

#define TO_STR_INNER(X) #X
#define TO_STR(X) TO_STR_INNER(X)

#define SOURCE_LOC                                                             \
    __FILE__                                                                   \
    " : " TO_STR(__LINE__)

/// Basic assertion.
// clang-format off
#define assert(X)                \
    if (!(X)) {                  \
        std::cout <<             \
            "assertion failed: " \
            #X                   \
            "\n"                 \
            SOURCE_LOC           \
            "\n";                \
        std::exit(1);            \
    }
// clang-format on

/// Basic assertion with a log message.
// clang-format off
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
// clang-format on

/// Unimplemented indicator.
// clang-format off
#define unimplemented std::cout <<          \
    "control reached unimplemented code:\n" \
    SOURCE_LOC                              \
    "\n";                                   \
    std::exit(1);
// clang-format on

// https://en.cppreference.com/w/cpp/utility/unreachable
[[noreturn]] static inline void unreachable_internal() {
#if defined(_MSC_VER) && !defined(__clang__)  // MSVC
    __assume(false);
#else  // GCC, Clang
    __builtin_unreachable();
#endif
}

// clang-format off
#define unreachable unreachable_internal();
// clang-format on
