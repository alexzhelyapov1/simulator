#include "intBitCache.h"
#include "machine.h"
#include "csr.h"
#include "entry.h"
#include <functional>
#include <iostream>
#include <memory>
#ifndef HART_H
#define HART_H
#define INST_CACHE_BIT_SIZE 10
#define INST_CACHE_BIT_SHIFT 0
#define TLB_BIT_SIZE 10
#define TLB_BIT_SHIFT 12
#define MAX_BB_SIZE 512

#define MODULE "Hart"
#include "logging.h"

namespace Machine {

class Hart;
class Instr;
using InstructionHandler = void (*)(Hart &, const std::shared_ptr<Instr>);
static Word opcodeMask = static_cast<Word>(127);

class Instr {
  public:
    Word instrCode;
    RegId rd, rs1, rs2;
    Word imm;
    InstructionHandler handler;
    RegValue mark;
    uWord opcodeFns;

    Instr(Word instrCode);
    Instr();

    operator int() const { return instrCode; }
};

enum class AccessType {
    READ,
    WRITE, 
    EXECUTE
};

enum class MMUMode {
    BASE_MODE
};


class LinearBlock
{
    std::vector<std::shared_ptr<Instr>> instrs;
    RegValue pc;
  public:
    inline std::shared_ptr<Instr> instr(RegValue blockPC) {
        return instrs[blockPC];
    }
    inline RegValue size(){
        return instrs.size() - 1;
    }
    LinearBlock(Hart &hart, const RegValue &PC);
    operator int() const { return pc; }
};

class Hart {
  private:
    RegValue PC{0};
    RegValue Regfile[32];
    Machine &machine;
    std::array<RegValue, 2> special_regs; // [0] - Page Table Pointer, [1] - MMU mode
    ControlStatusRegisters* csr; 

    std::shared_ptr<IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>> readTLB;
    std::shared_ptr<IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>> writeTLB;
    std::shared_ptr<IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>> executeTLB;

    std::shared_ptr<IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>> instCache;
    std::shared_ptr<IntBitCache<LinearBlock, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>> BBMemCache;
    bool free{true};
    RegValue numOfRunnedInstr{0};

    std::shared_ptr<IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>> getTLB(AccessType access_type) {
        switch (access_type) {
            case AccessType::READ:  return readTLB;
            case AccessType::WRITE: return writeTLB;
            case AccessType::EXECUTE: return executeTLB;
            default: return nullptr;
        }
    }

  public:
    Hart(Machine &machine, const RegValue &PC) : machine(machine), PC(PC) {
        readTLB = std::make_shared<IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>>();
        writeTLB = std::make_shared<IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>>();
        executeTLB = std::make_shared<IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>>();

        instCache = std::shared_ptr<IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>>(
            new IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>());
        BBMemCache = std::shared_ptr<IntBitCache<LinearBlock, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>>(
            new IntBitCache<LinearBlock, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>());
        Regfile[0] = 0;
    }
    Hart(Machine &machine) : machine(machine) {
        readTLB = std::make_shared<IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>>();
        writeTLB = std::make_shared<IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>>();
        executeTLB = std::make_shared<IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>>();
        
        instCache = std::shared_ptr<IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>>(
            new IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>());
        BBMemCache = std::shared_ptr<IntBitCache<LinearBlock, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>>(
            new IntBitCache<LinearBlock, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>());
        Regfile[0] = 0;
    }
    ~Hart() {}

    const bool &GetStatus() { return free; }

    std::shared_ptr<Instr> decode(const Word &instrCode);
    void RunSimpleInterpreterWithInstCache();
    void RunInterpreterWithBBCache();

    void setPC(const RegValue &pc) {
        PC = pc;
        Log(LogLevel::DEBUG, std::string("Set PC with: ") + std::to_string(PC));
    }

    const RegValue &getPC() {
        Log(LogLevel::DEBUG, std::string("Get PC: ") + std::to_string(PC));
        return PC;
    }

    RegValue getReg(const RegId &reg) {
        Log(LogLevel::DEBUG, std::string("Get Register: ") + std::to_string(reg) + " val: " + std::to_string(Regfile[reg]));
        return Regfile[reg];
    }

    void setReg(const RegId &reg, const RegValue &val) {
        if (reg == 0) {
            return;
        }
        Regfile[reg] = val;
        Log(LogLevel::DEBUG, std::string("Set Register: ") + std::to_string(reg) + " with val: " + std::to_string(val));
    }

    inline void exceptionReturn();
    inline const RegValue &GetNumOfRunInstr() { return numOfRunnedInstr; }

    inline const RegValue &MMU(RegValue &vaddress, AccessType accessFlag);

    template <typename ValType> ValType loadMem(RegValue address) {
        auto hostAddress = MMU(address, AccessType::READ);
        return machine.loadMem<ValType>(hostAddress);
    }

    template <typename ValType> ValType loadtoExec(RegValue address) {
        auto hostAddress = MMU(address, AccessType::EXECUTE);
        return machine.loadMem<ValType>(hostAddress);
    }

    template <typename ValType> void storeMem(RegValue address, ValType val) {
        auto hostAddress = MMU(address, AccessType::WRITE);
        machine.storeMem(hostAddress, val);
    }

    void handleInterrupt() {

    }
    friend class Machine;
};

} // namespace Machine


#undef MODULE

#endif