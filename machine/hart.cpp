#include "hart.h"
#include "gen_func.h"
#include "machine.h"
#include <sstream>

#define MODULE "Hart"
#include "logging.h"

namespace Machine {

Instr::Instr(Word instrCode) {
    auto decodeInfo = decodeMap[opcodeMask & instrCode];
    auto decodeArr = decodeInfo.first;
    auto decodeImmFunc = decodeInfo.second;
    Log(LogLevel::DEBUG, std::string("opode + Fns: ") + std::to_string(decodeArr[0] & instrCode));
    handler = instructionMap[decodeArr[0] & instrCode];
    imm = decodeImmFunc(instrCode);
    Log(LogLevel::DEBUG, std::string("Decode imm: ") + std::to_string(imm));
    rd = (instrCode & decodeArr[1]) >> decodeArr[2];
    rs1 = (instrCode & decodeArr[3]) >> decodeArr[4];
    rs2 = (instrCode & decodeArr[5]) >> decodeArr[6];
}



std::shared_ptr<Instr> Hart::decode(const Word &instrCode) {
    auto inst = instCache->get(instrCode);
    Log(LogLevel::DEBUG, std::string("Get Inst Code: ") + std::to_string(instrCode));
    if (inst != nullptr) {
        return inst;
    }
    inst = std::make_shared<Instr>(instrCode);
    instCache->put(inst);
    return inst;
}

void Hart::RunSimpleInterpreterWithInstCache() {
    free = false;
    while (true) {
        Log(LogLevel::DEBUG, std::string("Execute PC: ") + std::to_string(PC));
        auto inst = instMemCache->get(PC);
        if(inst == nullptr)
        {
            auto instCode = loadtoExec<Word>(PC);
            inst = decode(instCode);
            instMemCache->put(inst);
        }
        inst->handler(*this, inst);
        PC += sizeof(Word);
        numOfRunnedInstr++;
        Log(LogLevel::DEBUG, std::string(""));
    }
}

inline void Hart::exceptionReturn() {
    Log(LogLevel::DEBUG, std::string("EXCEPTION RETURN"));
    throw std::runtime_error("EXCEPTION RETURN FROM HART");
}

inline const RegValue &Hart::MMU(RegValue &vaddress, AccessType accessFlag) {
    // get VPN
    auto vPN = vaddress & ~0xFFF;
    // get Offset
    auto offset = vaddress & 0xFFF;

    // go to TLB by access flag
    if (accessFlag == AccessType::READ) {

    }

    // page table

    // make physical address
    // update tlb

    // return paddress + status

    return vaddress;
}

} // namespace Machine