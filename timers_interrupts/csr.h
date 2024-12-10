#pragma once
#include <cstdint>

using RegValue = uint64_t;

class ControlStatusRegisters {
    private:
    RegValue mip; // Machine Interrupt Pending
    RegValue mie; // Machine Interrupt Enable
    RegValue mstatus; // Machine Status Register

    public:
    RegValue& get_mstatus() {
        return mstatus;
    }
};

