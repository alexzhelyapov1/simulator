#include "intBitCache.h"
#include "machine.h"
#include "entry.h"
#include "timers.h"
#include <functional>
#include <iostream>
#include <memory>
#ifndef HART_H
#define HART_H
#define INST_CACHE_BIT_SIZE 10
#define INST_CACHE_BIT_SHIFT 0
#define TLB_BIT_SIZE 10
#define TLB_BIT_SHIFT 12
#define MAX_TICK 10000

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

    Instr(Word instrCode);

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

enum class SpecialRegs : RegValue {
    MIP = 2,
    MIE = 3, 
    MSTATUS = 4,
    MTIME = 5, 
    MTIMECMP = 6
};

class Hart {
  private:
    RegValue PC{0};
    RegValue Regfile[32];
    Machine &machine;
    std::array<RegValue, 4096> special_regs; // [0] - Page Table Pointer, [1] - MMU mode

    std::vector<Timer> globalTimers; 
    std::vector<Timer> currentTimers;
    Timer globalTimer{0, MAX_TICK};

    std::shared_ptr<IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>> readTLB;
    std::shared_ptr<IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>> writeTLB;
    std::shared_ptr<IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>> executeTLB;

    std::shared_ptr<IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>> instCache;
    std::shared_ptr<IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>> instMemCache;
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
        instMemCache = std::shared_ptr<IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>>(
            new IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>());
        Regfile[0] = 0;
    }
    Hart(Machine &machine) : machine(machine) {
        readTLB = std::make_shared<IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>>();
        writeTLB = std::make_shared<IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>>();
        executeTLB = std::make_shared<IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>>();
        
        instCache = std::shared_ptr<IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>>(
            new IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>());
        instMemCache = std::shared_ptr<IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>>(
            new IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>());
        Regfile[0] = 0;
    }
    ~Hart() {}

    const bool &GetStatus() { return free; }

    std::shared_ptr<Instr> decode(const Word &instrCode);
    void RunSimpleInterpreterWithInstCache();

    void setPC(const RegValue &pc);
    const RegValue &getPC();
    RegValue getReg(const RegId &reg);
    void setReg(const RegId &reg, const RegValue &val);

    void exceptionReturn();
    const RegValue &GetNumOfRunInstr() { return numOfRunnedInstr; }

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
        // set regs + setPC
    }

    inline RegValue getSpecialReg(SpecialRegs idx) {
        return special_regs[(int)idx];
    }

    inline void setSpecialReg(SpecialRegs idx, RegValue Value) {
        special_regs[(int)idx] = Value;
    }

    inline void incrementTimer() {
        // вектор глобальный таймеров
        // текущий лист таймеров где всегда меньше 10000

        // на каждом такте вычитать current у таймеров из currentTimers 
        // флаг в структуру - конечный таймер или его перезапускать
        // после handleInterrupt in current делаем set max timer если таймер бесконечный

        setSpecialReg(SpecialRegs::MTIME, getSpecialReg(SpecialRegs::MTIME) + 1);

        if (getSpecialReg(SpecialRegs::MTIME) >= getSpecialReg(SpecialRegs::MTIMECMP) && getSpecialReg(SpecialRegs::MTIMECMP) != 0) {
            RegValue mip_value = getSpecialReg(SpecialRegs::MIP); 
            mip_value |= (1 << 7);
            setSpecialReg(SpecialRegs::MIP, mip_value);

            handleInterrupt();
            setSpecialReg(SpecialRegs::MTIMECMP, 0);
        }
    }

    void createTimer(uint64_t max, bool isfinite) {
        if(max > MAX_TICK) {
            globalTimers.push_back(Timer{max, max, isfinite});
        }

    }

    friend class Machine;
};

} // namespace Machine

#endif