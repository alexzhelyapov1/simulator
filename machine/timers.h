#pragma once
#include <cstdint>

struct Timer {
    uint64_t current_;
    uint64_t maximum_;
    bool isfinite = true;
};