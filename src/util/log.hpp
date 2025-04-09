#pragma once

#include "format.hpp"

class Log {
public:
    enum class Level {
        Fatal = 0,
        Error,
        Warning,
        Info,
        Debug
    };

    template <typename... Targs>
    static void d(const char* format_string, const Targs&... args);

    template <typename... Targs>
    static void i(const char* format_string, const Targs&... args);

    template <typename... Targs>
    static void w(const char* format_string, const Targs&... args);

    template <typename... Targs>
    static void e(const char* format_string, const Targs&... args);

    template <typename... Targs>
    static void f(const char* format_string, const Targs&... args);

    static constexpr Level level = Level::Debug;
};

template <typename... Targs>
void Log::d(const char* format_string, const Targs&... args) {
    if constexpr (Log::level >= Log::Level::Debug)
        println((std::string("{}\t| ") + format_string).c_str(),
                Log::Level::Debug,
                args...);
}

template <typename... Targs>
void Log::i(const char* format_string, const Targs&... args) {
    if constexpr (Log::level >= Log::Level::Info)
        println((std::string("{}\t| ") + format_string).c_str(),
                Log::Level::Info,
                args...);
}

template <typename... Targs>
void Log::w(const char* format_string, const Targs&... args) {
    if constexpr (Log::level >= Log::Level::Warning)
        println((std::string("{}\t| ") + format_string).c_str(),
                Log::Level::Warning,
                args...);
}

template <typename... Targs>
void Log::e(const char* format_string, const Targs&... args) {
    if constexpr (Log::level >= Log::Level::Error)
        eprintln((std::string("{}\t| ") + format_string).c_str(),
                 Log::Level::Error,
                 args...);
}

template <typename... Targs>
void Log::f(const char* format_string, const Targs&... args) {
    if constexpr (Log::level >= Log::Level::Fatal)
        eprintln((std::string("{}\t| ") + format_string).c_str(),
                 Log::Level::Fatal,
                 args...);
}

template <>
struct FormatWriter<Log::Level> {
    static void write(const Log::Level& label, StringBuffer& sb) {
        switch (label) {
        case Log::Level::Debug:
            sb.append("DEBUG", 5);
            return;
        case Log::Level::Info:
            sb.append("INFO", 4);
            return;
        case Log::Level::Warning:
            sb.append("WARNING", 7);
            return;
        case Log::Level::Error:
            sb.append("ERROR", 5);
            return;
        case Log::Level::Fatal:
            sb.append("FATAL", 5);
            return;
        default:
            unreachable;
        }
    }
};
