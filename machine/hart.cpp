#include "hart.h"
#include "gen_func.h"
#include "machine.h"

namespace Machine {

RegValue Hart::getReg(const RegId &reg) {
    Log(LogLevel::DEBUG, std::string("Get Register: ") + std::to_string(reg) + " val: " + std::to_string(Regfile[reg]));
    return Regfile[reg];
}

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

void Hart::setReg(const RegId &reg, const RegValue &val) {
    if (reg == 0) {
        return;
    }
    Regfile[reg] = val;
    Log(LogLevel::DEBUG, std::string("Set Register: ") + std::to_string(reg) + " with val: " + std::to_string(val));
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

void Hart::setPC(const RegValue &pc) {
    PC = pc;
    Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "Set PC: 0x" << PC).str());
}

const RegValue &Hart::getPC() {
    Log(LogLevel::DEBUG, std::string("Get PC: ") + std::to_string(PC));
    return PC;
}

void Hart::RunSimpleInterpreterWithInstCache() {
    free = false;
    while (true) {
        Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "Execute PC: 0x" << PC).str());
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

void Hart::exceptionReturn() {
    Log(LogLevel::DEBUG, std::string("EXCEPTION RETURN"));
    throw std::runtime_error("EXCEPTION RETURN FROM HART");
}

RegValue Hart::MMU(RegValue vaddress, AccessType accessFlag) {
    // get VPN
    const RegValue VPN = vaddress >> 12;

    // get Offset
    const RegValue offset = vaddress & 0xFFF;

    // go to TLB by access flag
    auto tlb = getTLB(accessFlag);

    auto found_in_tlb = tlb->get(vaddress & ~0xFFF);
    if (found_in_tlb != nullptr) {
        #ifndef NDEBUG
        if (found_in_tlb->paddr & 0xFFF != 0) {
            throw std::runtime_error("Paddr should be aligned at 4kb.");
        }
        #endif

        Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "TLB vaddr: 0x" << vaddress << " => paddr: 0x"
            << found_in_tlb->paddr + offset << ", access: 0x" << static_cast<int64_t>(accessFlag)).str());
        return found_in_tlb->paddr + offset;
    }

    // SV39
    const uint32_t VPN_1 = (VPN >> 18) & 0x1FF;
    const uint32_t VPN_2 = (VPN >> 9) & 0x1FF;
    const uint32_t VPN_3 = VPN & 0x1FF;


    // Find paddr
    RegValue *page_table_0 = nullptr;
    RegValue *page_table_1 = nullptr;
    RegValue *page_table_2 = nullptr;

    page_table_0 = reinterpret_cast<RegValue *>(machine.mem->GetHostAddr(getSATP() & ~0xFFF));

    if (page_table_0[VPN_1] == 0) {
        throw std::runtime_error("Page fault (page_table_1 = 0).");
    }
    page_table_1 = reinterpret_cast<RegValue *>(machine.mem->GetHostAddr(page_table_0[VPN_1]));

    if (page_table_1[VPN_2] == 0) {
        throw std::runtime_error("Page fault (page_table_2 = 0).");
    }
    page_table_2 = reinterpret_cast<RegValue *>(machine.mem->GetHostAddr(page_table_1[VPN_2]));

    RegValue paddress = page_table_2[VPN_3];

    // Check for access rights
    if (paddress & static_cast<RegValue>(accessFlag) == 0) {
        throw std::runtime_error("Page fault (bad access rights).");
    }

    // update tlb
    std::shared_ptr<TLBEntry> new_tlb_entry(new TLBEntry(vaddress & ~0xFFF, paddress & ~0xFFF));
    tlb->put(new_tlb_entry);

    Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "MMU vaddr: 0x" << vaddress << " => paddr: 0x"
        << (paddress & ~0xFFF) + offset).str());

    // return paddress
    return (paddress & ~0xFFF) + offset;
}

} // namespace Machine