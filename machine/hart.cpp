#include <assert.h>
#include "hart.h"
#include "gen_func.h"
#include "machine.h"

namespace Machine {

Instr::Instr(uWord instrCode) {
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
    Instr instr;
    do {
        instCode = hart.loadtoExec<uWord>(PC + shift);
        instr = hart.decode(instCode);
        instrs.push_back(instr);
        shift += sizeof(uWord);
    } while(checkBBEndInstr(instr.opcodeFns) && (instrs.size() - 1 < MAX_BB_SIZE));
    instrs.push_back(Instr());
    Log(LogLevel::DEBUG, std::string("End of decoding BB"));
    Log(LogLevel::DEBUG, std::string(""));
}

Instr::Instr() {
    instrCode = 0;
    handler = nullptr;
    mark = SIZE_OF_INSTRS;
}

Instr Hart::decode(const uWord &instrCode) {
    Log(LogLevel::DEBUG, std::string("Get Inst Code: ") + std::to_string(instrCode));
    auto inst = Instr(instrCode);
    return inst;
}

void Hart::RunSimpleInterpreterWithInstCache() {
    free = false;
    while (true) {
        Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "Execute PC: 0x" << PC).str());
        auto instCode = loadtoExec<uWord>(PC);
        auto inst = decode(instCode);
        inst.handler(*this, &inst);
        PC += sizeof(uWord);
        numOfRunnedInstr++;
        Log(LogLevel::DEBUG, std::string(""));
    }
}

void Hart::RunInterpreterWithBBCache() {
    free = false;
#ifdef SIM_TIME
    startSimTime = std::chrono::high_resolution_clock::now();
#endif
    while (true) {
        Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "Decode BB PC: 0x" << PC).str());
        auto bb = BBMemCache.get(PC);
        if(bb == nullptr)
        {
            bb = std::shared_ptr<LinearBlock>(new LinearBlock(*this, PC));
            BBMemCache.put(bb);
        }
        ExecuteLinearBlock(*this, bb.get());
        numOfRunnedInstr += bb->size() - 1;
        Log(LogLevel::DEBUG, std::string(""));
    }
}

inline void Hart::exceptionReturn(const std::string str, LinearBlock *bb) {
    if(bb != nullptr)
    {
        numOfRunnedInstr += bb->size() - 1;
    }
    Log(LogLevel::DEBUG, std::string("EXCEPTION RETURN") + str);
    throw std::runtime_error(std::string("EXCEPTION RETURN FROM HART") + str);
}

template <AccessType accessFlag>
RegValue Hart::MMU(RegValue vaddress) {
    if (getSatpMmuMode() == SATP_MMU_MODE::NO_MMU) {
        Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "NO MMU vaddr: 0x" << vaddress << " => paddr: 0x"
            << vaddress).str());
        return vaddress;
    }
    else {
        Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "MMU working! Mode: 0x"
            << static_cast<int64_t>(getSatpMmuMode())).str());
    }
    const RegValue offset = vaddress & 0xFFF;

    // Try to find in TLB
    auto& tlb = getTLB<accessFlag>();
    auto found_in_tlb = tlb.get(vaddress & ~0xFFF);
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

    for (int i = 0; i < (1 << 9); ++i) {
        if (current_page_table[i] != 0) {
            Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "Non empty PTE in RPT index: 0x" << i).str());
        }
    }

    const RegValue VPN = vaddress >> 12;
    assert(nested_transitions == 2 || nested_transitions == 3);
    for (int i = 0; i < nested_transitions; i++) {
        RegValue VPN_i = (VPN >> 9 * (nested_transitions - i)) & ((1 << 9) - 1);
        if (current_page_table[VPN_i] == 0) {
            Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "Unknown vaddr: 0x" << vaddress
                << ", nested tables: 0x" << nested_transitions
                << ", current table index: 0x" << i).str());
            throw std::runtime_error("Page fault.");
        }
        current_page_table = reinterpret_cast<RegValue *>(machine.mem->GetHostAddr(current_page_table[VPN_i]));
    }

    RegValue paddress = current_page_table[VPN & ((1 << 9) - 1)];

    // Check for access rights
    if ((paddress & static_cast<RegValue>(accessFlag)) == 0) {
        Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "Paddress: 0x" << paddress
                << ", access: 0x" << static_cast<RegValue>(accessFlag)).str());
        throw std::runtime_error("Page fault (bad access rights).");
    }

    // Update TLB
    std::shared_ptr<TLBEntry> new_tlb_entry(new TLBEntry(vaddress & ~0xFFF, paddress & ~0xFFF));
    tlb.put(new_tlb_entry);

    Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "MMU vaddr: 0x" << vaddress << " => paddr: 0x"
        << (paddress & ~0xFFF) + offset).str());

    // Return paddress
    return (paddress & ~0xFFF) + offset;
}

#ifdef PLUGIN_ENABLED
void Hart::InitPluginCalls(void *pluginHandler)
{
    InitHandlers(pluginHandler);
    InitInstrPluginHandlers(*this, pluginHandler);
}
#endif

} // namespace Machine