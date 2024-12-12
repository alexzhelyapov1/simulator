// Before include this header you should define MODULE as string of current file. (e.g. #define MODULE "LOADER")
#pragma once
#include <string>
#include <sstream>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

void GlobalLog(LogLevel level, const std::string &msg);

// Log is ON
// static void Log(LogLevel level, const std::string &msg) {
//     GlobalLog(level, "[" + std::string(MODULE) + "]: " + msg);
// }

// Log is OFF
#define Log(level, msg);