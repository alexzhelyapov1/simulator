#include <iostream>
#include "hart.h"

extern "C"
{
    void getReg(Machine::Hart *hart, Machine::RegId *reg, Machine::Instr *instr)
    {
        std::cout << "PLUGIN GETREG: " << std::to_string(*reg) << " VAL: " << hart->getReg(*reg) << std::endl; 
    }
    void getPC(Machine::Hart *hart, Machine::RegValue *pc, Machine::Instr *instr)
    {
        std::cout << "PLUGIN GETPC: " << std::to_string(*pc) << std::endl; 
    }
    void ADDI(Machine::Hart *hart, Machine::RegValue *reg, Machine::Instr *instr)
    {
        std::cout << "PLUGIN ADDI REG1: " << std::to_string(instr->rs1) << 
        " IMM: " << std::to_string(instr->imm) << std::endl; 
    }
}