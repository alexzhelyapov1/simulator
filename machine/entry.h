#pragma once
#include <cstdint>
#include <cstddef>
#include "machine.h"


struct TLBEntry {
    RegValue vaddr;
    RegValue paddr;

    TLBEntry(RegValue vaddr, RegValue paddr): vaddr(vaddr), paddr(paddr){}

    operator size_t() const {
        return static_cast<size_t> (vaddr);
    }
};