#include <iostream>
#include "hart.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

extern "C"
{
    void setReg(Machine::Hart *hart, Machine::RegId *reg, Machine::Instr *instr)
    {
        // if((*reg) == 0) return;
        std::cout << std::hex << std::setw(16) << std::setfill('0') << hart->getReg(*reg) << std::endl;
    }
}