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
    opcodeFns = decodeArr[0] & instrCode;
    Log(LogLevel::DEBUG, std::string("opode + Fns: ") + std::to_string(opcodeFns));
    handler = instructionMap[opcodeFns];
    imm = decodeImmFunc(instrCode);
    Log(LogLevel::DEBUG, std::string("Decode imm: ") + std::to_string(imm));
    rd = (instrCode & decodeArr[1]) >> decodeArr[2];
    rs1 = (instrCode & decodeArr[3]) >> decodeArr[4];
    rs2 = (instrCode & decodeArr[5]) >> decodeArr[6];
    mark = instructionMarkMap[opcodeFns];
}

LinearBlock::LinearBlock(Hart &hart, const RegValue &PC) : pc(PC) {
    RegValue instCode;
    RegValue shift = 0;
    std::shared_ptr<Instr> instr;
    do {
        instCode = hart.loadtoExec<Word>(PC + shift);
        instr = hart.decode(instCode);
        instrs.push_back(instr);
        shift += sizeof(Word);
    } while(checkBBEndInstr(instr->opcodeFns) && instrs.size() < MAX_BB_SIZE);
    instrs.push_back(std::shared_ptr<Instr>(new Instr()));
    Log(LogLevel::DEBUG, std::string("End of decoding BB"));
    Log(LogLevel::DEBUG, std::string(""));
}

Instr::Instr() {
    instrCode = 0;
    handler = nullptr;
    mark = SIZE_OF_INSTRS;
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
        auto instCode = loadtoExec<Word>(PC);
        auto inst = decode(instCode);
        inst->handler(*this, inst);
        PC += sizeof(Word);
        numOfRunnedInstr++;
        Log(LogLevel::DEBUG, std::string(""));
    }
}

void Hart::RunInterpreterWithBBCache() {
    free = false;
    while (true) {
        Log(LogLevel::DEBUG, std::string("Execute PC: ") + std::to_string(PC));
        auto bb = BBMemCache->get(PC);
        if(bb == nullptr)
        {
            bb = std::shared_ptr<LinearBlock>(new LinearBlock(*this, PC));
            BBMemCache->put(bb);
        }
        ExecuteLinearBlock(*this, bb);
        numOfRunnedInstr += bb->size();
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