#include <array>
#include <iostream>

#define MODULE "Logging"
#include "logging.h"


static const std::array<const std::string, 5> LogLevelNames = {
    "[DEBUG]:",
    "[INFO]:",
    "[WARNING]:",
    "[ERROR]:",
    "[CRITICAL]:",
};


void GlobalLog(LogLevel level, const std::string &msg) {
    std::cout << LogLevelNames[static_cast<int>(level)] << msg << std::endl;
}
