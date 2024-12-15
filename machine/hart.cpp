#include "hart.h"
#include "gen_func.h"
#include "machine.h"

namespace Machine {

Instr::Instr(Word instrCode) {
    auto decodeInfo = decodeMap[opcodeMask & instrCode];
    auto decodeArr = decodeInfo.first;
    auto decodeImmFunc = decodeInfo.second;
    opcodeFns = decodeArr[0] & instrCode;
    auto instr_name = logMap[opcodeFns];
    Log(LogLevel::DEBUG, std::string("opсode + Fns: ") + std::to_string(opcodeFns));
    Log(LogLevel::DEBUG, std::string("Runed Instruction: ") + (instr_name));
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
        Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "Execute PC: 0x" << PC).str());
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

RegValue Hart::MMU(RegValue vaddress, AccessType accessFlag) {
    const RegValue offset = vaddress & 0xFFF;

    // Try to find in TLB
    auto tlb = getTLB(accessFlag);
    auto found_in_tlb = tlb->get(vaddress & ~0xFFF);
    if (found_in_tlb != nullptr) {
        #ifndef NDEBUG
        if ((found_in_tlb->paddr & 0xFFF) != 0) {
            throw std::runtime_error("Paddr should be aligned at 4kb.");
        }
        #endif

        Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "TLB vaddr: 0x" << vaddress << " => paddr: 0x"
            << found_in_tlb->paddr + offset << ", access: 0x" << static_cast<int64_t>(accessFlag)).str());

        // Return paddr from TLB
        return found_in_tlb->paddr + offset;
    }

    // Not found in TLB. Translate address
    static int nested_transitions = (static_cast<int64_t>(getSatpMmuMode()) >> 60) - 1;
    RegValue *current_page_table = reinterpret_cast<RegValue *>(machine.mem->GetHostAddr(getRootPageTablePaddr()));

    const RegValue VPN = vaddress >> 12;
    for (int i = 0; i < nested_transitions; i++) {
        RegValue VPN_i = (VPN >> 9 * (nested_transitions - i)) & ((1 << 9) - 1);
        if (current_page_table[VPN_i] == 0) {
            throw std::runtime_error("Page fault.");
        }
        current_page_table = reinterpret_cast<RegValue *>(machine.mem->GetHostAddr(current_page_table[VPN_i]));
    }

    RegValue paddress = current_page_table[VPN & ((1 << 9) - 1)];

    // Check for access rights
    if ((paddress & static_cast<RegValue>(accessFlag)) == 0) {
        throw std::runtime_error("Page fault (bad access rights).");
    }

    // Update TLB
    std::shared_ptr<TLBEntry> new_tlb_entry(new TLBEntry(vaddress & ~0xFFF, paddress & ~0xFFF));
    tlb->put(new_tlb_entry);

    Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "MMU vaddr: 0x" << vaddress << " => paddr: 0x"
        << (paddress & ~0xFFF) + offset).str());

    // Return paddress
    return (paddress & ~0xFFF) + offset;
}

} // namespace Machine