#pragma once
#include <cstdint>
#include <cstddef>

using VirtualAddress = uint64_t;
using PhysicalAddress = uint64_t;

struct TLBEntry {
    VirtualAddress vaddr;
    PhysicalAddress paddr;

    operator size_t() const {
        return static_cast<size_t> (vaddr);
    }
};