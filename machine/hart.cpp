#include "hart.h"
#include "machine.h"
#include "gen_func.h"

namespace Machine {

#define SIMULATION_LOG

#ifdef SIMULATION_LOG
#include <iostream>
#endif

#ifdef SIMULATION_LOG
void SimulationLog(std::string log) { std::cout << "[SIMULATION LOG] " << log << std::endl; }
#endif

RegValue Hart::getReg(const RegId &reg) {
#ifdef SIMULATION_LOG
    SimulationLog(std::string("Get Register: ") + std::to_string(reg) + " val: " + std::to_string(Regfile[reg]));
#endif
    return Regfile[reg];
}

Instr::Instr(Word instrCode)
{
    auto decodeInfo = decodeMap[opcodeMask & instrCode];
    auto decodeArr = decodeInfo.first;
    auto decodeImmFunc = decodeInfo.second;
    handler = instructionMap[decodeArr[0] & instrCode];
    imm = decodeImmFunc(instrCode);
    std::cout << "imm: " << (imm) << std::endl;
    rd = (instrCode & decodeArr[1]) >> decodeArr[2];
    rs1 = (instrCode & decodeArr[3]) >> decodeArr[4];
    rs2 = (instrCode & decodeArr[5]) >> decodeArr[6];
}

void Hart::setReg(const RegId &reg, const RegValue &val) {
    if (reg == 0) {
        return;
    }
    Regfile[reg] = val;
#ifdef SIMULATION_LOG
    SimulationLog(std::string("Set Register: ") + std::to_string(reg) + " with val: " + std::to_string(val));
#endif
}

std::shared_ptr<Instr> Hart::decode(const Word &instrCode) {
    auto inst = instCache->get(instrCode);
#ifdef SIMULATION_LOG
    SimulationLog(std::string("Get Inst Code: ") + std::to_string(instrCode));
#endif
    if (inst != nullptr) {
        return inst;
    }
    inst = std::make_shared<Instr>(instrCode);
    instCache->put(inst);
    return inst;
}

void Hart::setPC(const RegValue &pc) {
    PC = pc;
#ifdef SIMULATION_LOG
    SimulationLog(std::string("Set PC with: ") + std::to_string(PC));
    std::cout << "Set PC: " << std::hex << PC << std::dec << std::endl;
#endif
}

const RegValue &Hart::getPC() {
#ifdef SIMULATION_LOG
    SimulationLog(std::string("Get PC: ") + std::to_string(PC));
#endif
    return PC;
}

void Hart::RunSimpleInterpreterWithInstCache() {
    free = false;
    while (true) {
        auto instCode = loadMem<Word>(PC);
        auto inst = decode(instCode);
        inst->handler(*this, inst);
        PC += sizeof(Word);
    }
}

void Hart::exceptionReturn() {
#ifdef SIMULATION_LOG
    SimulationLog(std::string("EXCEPTION RETURN"));
#endif
    throw std::runtime_error("EXCEPTION RETURN FROM HART");
}

inline const RegValue &Hart::MMU(RegValue &vaddress) {
    // TODO insert MMU here
    return vaddress;
}

} // namespace Machine