#pragma once
#include <cstdint>
#include <cstddef>
#include "machine.h"


struct TLBEntry {
    Machine::RegValue vaddr;
    Machine::RegValue paddr;

    TLBEntry(Machine::RegValue vaddr, Machine::RegValue paddr): vaddr(vaddr), paddr(paddr){}

    operator size_t() const {
        return static_cast<size_t> (vaddr);
    }
};