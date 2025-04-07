#pragma once

#include <string>

#include "common.hpp"
#include "formatter.hpp"
#include "stringbuffer.hpp"

/// @brief Formatter wrapper that creates a new std::string.
/// @tparam ...Targs Format string argument types.
/// @param format_string Format string.
/// @param ...args Format string arguments.
/// @return Formatted string.
template <typename... Targs>
std::string formatted(const char* format_string, const Targs&... args) {
    Formatter formatter(format_string, std::strlen(format_string));
    StringBuffer sb;

    try {
        formatter.format(sb, args...);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << '\n';
        return std::string();
    }

    return std::string(sb.str());
}

/// @brief Formatter wrapper that fills a StringBuffer.
/// @tparam ...Targs Format string argument types.
/// @param format_string Format string.
/// @param ...args Format string arguments.
/// @return Formatted string.
template <typename... Targs>
void format(StringBuffer& sb, const char* format_string, const Targs&... args) {
    Formatter formatter(format_string, std::strlen(format_string));
    try {
        formatter.format(sb, args...);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << '\n';
        return;
    }
}

/// @brief Prints a formatted string followed by a newline.
/// @tparam ...Targs Format string argument types.
/// @param format_string Format string.
/// @param ...args Format string arguments.
template <typename... Targs>
void println(const char* format_string, const Targs&... args) {
    StringBuffer sb;
    format(sb, format_string, args...);
    std::cout << sb.str() << '\n';
}

/// @brief Prints a formatted string.
/// @tparam ...Targs Format string argument types.
/// @param format_string Format string.
/// @param ...args Format string arguments.
template <typename... Targs>
void print(const char* format_string, const Targs&... args) {
    StringBuffer sb;
    format(sb, format_string, args...);
    std::cout << sb.str();
}

/// @brief Prints a formatted string followed by a newline to stderr.
/// @tparam ...Targs Format string argument types.
/// @param format_string Format string.
/// @param ...args Format string arguments.
template <typename... Targs>
void eprintln(const char* format_string, const Targs&... args) {
    StringBuffer sb;
    format(sb, format_string, args...);
    std::cerr << sb.str() << std::endl;
}

/// @brief Prints a formatted string to stderr.
/// @tparam ...Targs Format string argument types.
/// @param format_string Format string.
/// @param ...args Format string arguments.
template <typename... Targs>
void eprint(const char* format_string, const Targs&... args) {
    StringBuffer sb;
    format(sb, format_string, args...);
    std::cerr << sb.str() << std::flush;
}
