#include "intBitCache.h"
#include "machine.h"
#include <functional>
#include <iostream>
#include <memory>
#ifndef HART_H
#define HART_H
#define INST_CACHE_BIT_SIZE 10
#define INST_CACHE_BIT_SHIFT 0
#define TLB_BIT_SIZE 10
#define TLB_BIT_SHIFT 12

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

class Hart {
  private:
    RegValue PC{0};
    RegValue Regfile[32];
    Machine &machine;
    std::array<RegValue, 2> special_regs; // [0] - Page Table Pointer, [1] - MMU mode

    std::shared_ptr<IntBitCache<uint64_t, TLB_BIT_SIZE, TLB_BIT_SHIFT>> readTLB;
    std::shared_ptr<IntBitCache<uint64_t, TLB_BIT_SIZE, TLB_BIT_SHIFT>> writeTLB;
    std::shared_ptr<IntBitCache<uint64_t, TLB_BIT_SIZE, TLB_BIT_SHIFT>> executeTLB;

    std::shared_ptr<IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>> instCache;
    std::shared_ptr<IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>> instMemCache;
    bool free{true};
    RegValue numOfRunnedInstr{0};

    std::shared_ptr<IntBitCache<uint64_t, TLB_BIT_SIZE, TLB_BIT_SHIFT>> getTLB(AccessType access_type) {
        switch (access_type) {
            case AccessType::READ:  return readTLB;
            case AccessType::WRITE: return writeTLB;
            case AccessType::EXECUTE: return executeTLB;
            default: return nullptr;
        }
    }

  public:
    Hart(Machine &machine, const RegValue &PC) : machine(machine), PC(PC) {
        readTLB = std::make_shared<IntBitCache<uint64_t, TLB_BIT_SIZE, TLB_BIT_SHIFT>>();
        writeTLB = std::make_shared<IntBitCache<uint64_t, TLB_BIT_SIZE, TLB_BIT_SHIFT>>();
        executeTLB = std::make_shared<IntBitCache<uint64_t, TLB_BIT_SIZE, TLB_BIT_SHIFT>>();

        instCache = std::shared_ptr<IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>>(
            new IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>());
        instMemCache = std::shared_ptr<IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>>(
            new IntBitCache<Instr, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>());
        Regfile[0] = 0;
    }
    Hart(Machine &machine) : machine(machine) {
        readTLB = std::make_shared<IntBitCache<uint64_t, TLB_BIT_SIZE, TLB_BIT_SHIFT>>();
        writeTLB = std::make_shared<IntBitCache<uint64_t, TLB_BIT_SIZE, TLB_BIT_SHIFT>>();
        executeTLB = std::make_shared<IntBitCache<uint64_t, TLB_BIT_SIZE, TLB_BIT_SHIFT>>();
        
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

    friend class Machine;
};

} // namespace Machine

#endif