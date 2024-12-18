#ifndef MODULE
    #define MODULE "Hart"
    #include "logging.h"
#endif
#include "intBitCache.h"
#include <chrono>
#include "machine.h"
#include "csr.h"
#include "entry.h"
#include <memory>
#include <functional>
#include <map>
#include <dlfcn.h>
#ifndef HART_H
#define HART_H
#define INST_CACHE_BIT_SIZE 10
#define INST_CACHE_BIT_SHIFT 0
#define TLB_BIT_SIZE 10
#define TLB_BIT_SHIFT 12
#define MAX_BB_SIZE 512

#define SIM_TIME

namespace Machine {

class Hart;
class Instr;
using InstructionHandler = void (*)(Hart &, Instr *);
static Word opcodeMask = static_cast<Word>(127);

class Instr {
  public:
    uWord instrCode;
    RegId rd, rs1, rs2;
    Word imm;
    InstructionHandler handler;
    RegValue mark;
    uWord opcodeFns;

    Instr(uWord instrCode);
    Instr();

    operator int() const { return instrCode; }
};

enum class AccessType {
    READ = 1,
    WRITE = 1 << 1,
    EXECUTE = 1 << 2
};

// For convenience value = number of page tables in this mode. SATP_MODE_SIZE = 4 allows to do this
enum class SATP_MMU_MODE: int64_t {
    SV39 = int64_t(3) << 60,
    SV48 = int64_t(4) << 60,
};


class LinearBlock
{
    std::vector<Instr> instrs;
    RegValue pc;
  public:
    inline Instr *instr(RegValue blockPC) {
        return &(instrs[blockPC]);
    }
    inline RegValue size(){
        return instrs.size();
    }
    LinearBlock(Hart &hart, const RegValue &PC);
    operator int() const { return pc; }
};

#ifdef PLUGIN_ENABLED
using PluginMethodHandler = void (*)(Hart *, RegValue *reg, Instr *instr);
#endif

class Hart {
  private:
    RegValue PC{0};
    RegValue Regfile[32];
    Machine &machine;
    ControlStatusRegisters* csr;
    RegValue special_regs[4096];
#ifdef SIM_TIME
    std::chrono::_V2::system_clock::time_point startSimTime; 
#endif
#ifdef PLUGIN_ENABLED
    std::map<std::string, PluginMethodHandler> handlers;
#endif
    IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT> readTLB;
    IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT> writeTLB;
    IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT> executeTLB;

    IntBitCache<LinearBlock, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT> BBMemCache;
    bool free{true};
    RegValue numOfRunnedInstr{0};

    template <AccessType accessFlag>
    IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>& getTLB() {
        if constexpr (accessFlag == AccessType::READ) {
            return readTLB;
        } 
        else if constexpr (accessFlag == AccessType::WRITE) {
            return writeTLB;
        } 
        else if constexpr (accessFlag == AccessType::EXECUTE) {
            return executeTLB;
        }
    }

#ifdef PLUGIN_ENABLED
    inline void InitHandlers(void *pluginHandler) {
        handlers["getReg"] = reinterpret_cast<PluginMethodHandler>(FindHandler(pluginHandler, "getReg"));
        handlers["setReg"] = reinterpret_cast<PluginMethodHandler>(FindHandler(pluginHandler, "setReg"));
        handlers["getPC"] = reinterpret_cast<PluginMethodHandler>(FindHandler(pluginHandler, "getPC"));
        handlers["setPC"] = reinterpret_cast<PluginMethodHandler>(FindHandler(pluginHandler, "setPC"));
    }
#endif

  public:
    Hart(Machine &machine, const RegValue &PC) : machine(machine), PC(PC) {
        readTLB = IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>();
        writeTLB = IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>();
        executeTLB = IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>();

        BBMemCache = IntBitCache<LinearBlock, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>();
        Regfile[0] = 0;
    }
    Hart(Machine &machine) : machine(machine) {
        readTLB = IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>();
        writeTLB = IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>();
        executeTLB = IntBitCache<TLBEntry, TLB_BIT_SIZE, TLB_BIT_SHIFT>();

        BBMemCache = IntBitCache<LinearBlock, INST_CACHE_BIT_SIZE, INST_CACHE_BIT_SHIFT>();
        Regfile[0] = 0;
    }

    inline RegValue getSpecialReg(int64_t idx) {
        return special_regs[idx];
    }

    inline void setSpecialReg(int64_t idx, RegValue Value) {
        special_regs[idx] = Value;
    }

    ~Hart() {}

#ifdef PLUGIN_ENABLED
    inline void *FindHandler(void *pluginHandler, std::string handlerName) {
        return dlsym(pluginHandler, handlerName.data());
    }

    void InitPluginCalls(void *pluginHandler);

    inline auto &GetHandlers()
    {
        return handlers;
    }
#endif

    const bool &GetStatus() { return free; }

    Instr decode(const uWord &instrCode);
    void RunSimpleInterpreterWithInstCache();
    void RunInterpreterWithBBCache();

    void setPC(const RegValue &pc) {
        PC = pc;
#ifdef PLUGIN_ENABLED
        auto handl = handlers["setPC"];
        if(handl != nullptr)
        {
            handl(this, &(const_cast<RegValue &>(pc)), nullptr);
        }
#endif
        Log(LogLevel::DEBUG, std::string("Set PC with: ") + std::to_string(PC));
    }

    const RegValue &getPC() {
        Log(LogLevel::DEBUG, std::string("Get PC: ") + std::to_string(PC));
#ifdef PLUGIN_ENABLED
        auto handl = handlers["getPC"];
        if(handl != nullptr)
        {
            handl(this, &(const_cast<RegValue &>(PC)), nullptr);
        }
#endif
        return PC;
    }

    RegValue getReg(const RegId &reg) {
        Log(LogLevel::DEBUG, std::string("Get Register: ") + std::to_string(reg) + " val: " + std::to_string(Regfile[reg]));
#ifdef PLUGIN_ENABLED
        auto handl = handlers["getReg"];
        if(handl != nullptr)
        {
            handl(this, &((RegValue &)const_cast<RegId &>(reg)), nullptr);
        }
#endif
        return Regfile[reg];
    }

    void setReg(const RegId &reg, const RegValue &val) {
        if (reg == 0) {
            return;
        }
        Regfile[reg] = val;
#ifdef PLUGIN_ENABLED
        auto handl = handlers["setReg"];
        if(handl != nullptr)
        {
            handl(this, &((RegValue &)const_cast<RegId &>(reg)), nullptr);
        }
#endif
        Log(LogLevel::DEBUG, std::string("Set Register: ") + std::to_string(reg) + " with val: " + std::to_string(val));
    }

    inline void exceptionReturn(const std::string str = "", LinearBlock *bb = nullptr);
    inline const RegValue &GetNumOfRunInstr() { return numOfRunnedInstr; }

    template<AccessType accessFlag>
    RegValue MMU(RegValue vaddress);

    template <typename ValType> ValType loadMem(RegValue address) {
        auto hostAddress = MMU<AccessType::READ>(address);
        return machine.loadMem<ValType>(hostAddress);
    }

    template <typename ValType> ValType loadtoExec(RegValue address) {
        auto hostAddress = MMU<AccessType::EXECUTE>(address);
        return machine.loadMem<ValType>(hostAddress);
    }

    template <typename ValType> void storeMem(RegValue address, ValType val) {
        auto hostAddress = MMU<AccessType::WRITE>(address);
        machine.storeMem(hostAddress, val);
    }

    inline void setSatp(RegValue value) {
        special_regs[0x180] = value;
    }

    inline SATP_MMU_MODE getSatpMmuMode() {
        return static_cast<SATP_MMU_MODE>(special_regs[0x180] & (uint64_t(0xFFFF) << 60));
    }

    inline RegValue getRootPageTablePaddr() {
        return (special_regs[0x180] & (int64_t(1) << 44) - 1) << 12;
    }

    void handleInterrupt() {

    }
#ifdef SIM_TIME
    inline auto getStartSimTime() {
        return startSimTime;
    }
#endif

    friend class Machine;
};

} // namespace Machine


#undef MODULE

#endif