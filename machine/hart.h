#include <memory>
#include <functional>
#include "machine.h"
#include "intBitCache.h"
#include <iostream>
#ifndef HART_H
#define HART_H
#define INST_CACHE_BIT_SIZE 10
#define INST_CACHE_BIT_SHIFT 0

namespace Machine 
{

class Hart;
class Instr;
using InstructionHandler = void (*)(Hart &, const std::shared_ptr<Instr>);
static Word opcodeMask = static_cast<Word>(127);

class Instr 
{
public:
    Word instrCode;
    RegId rd, rs1, rs2;
    Word imm;
    InstructionHandler handler;

    Instr(Word instrCode);

    Word cacheId()
    {
        return instrCode;
    }
};

class Hart
{
private:
    RegValue PC {0};
    RegValue Regfile[32];
    std::weak_ptr<Machine> machine;
    std::shared_ptr<IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>> instCache;
    bool free {true};
    
public:
    Hart(std::shared_ptr<Machine> machine, const RegValue &PC) : machine(machine), PC(PC) {
        instCache = std::shared_ptr<IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>>(new IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>());
        Regfile[0] = 0;
    }
    Hart(std::shared_ptr<Machine> machine) : machine(machine) {
        instCache = std::shared_ptr<IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>>(new IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>());
        Regfile[0] = 0;
    }
    ~Hart() {}

    const bool &GetStatus() {return free;} 

    std::shared_ptr<Instr> decode(const Word& instrCode);
    void RunSimpleInterpreterWithInstCache();

    void setPC(const RegValue &pc);
    const RegValue &getPC();
    RegValue getReg(const RegId &reg);
    void setReg(const RegId &reg, const RegValue &val);

    void exceptionReturn();

    inline const RegValue &MMU(RegValue &vaddress);

    template <typename ValType> ValType loadMem(RegValue address) {
        auto hostAddress = MMU(address);
        return machine.lock()->loadMem<ValType>(hostAddress);
    }

    template <typename ValType> void storeMem(RegValue address, ValType val) {
        auto hostAddress = MMU(address);
        machine.lock()->storeMem(hostAddress, val);
    }

    friend class Machine;
};

}

#endif