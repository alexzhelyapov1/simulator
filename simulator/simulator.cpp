#include "simulator.h"

namespace Simulator {

void Simulator::StartSimulationOnSimpleInterpreter(const std::string &filePath, const std::string &dumpMemPath) {
    std::shared_ptr<Machine::Hart> simHart = nullptr;
    for (auto hart : machine->GetHarts()) {
        if (hart->GetStatus()) {
            simHart = hart;
            break;
        }
    }
    if (simHart == nullptr) {
        simHart = machine->CreateHart();
    }

    AllocVirtualMemToStartProcess(simHart);

    auto pc = loader->loadElf(filePath, simHart);
    simHart->setPC(pc);
    if (dumpMemPath.size() != 0) {
        machine->DumpMem(dumpMemPath, pc, pc + 0x10000);
    }

    try {
        simHart->RunInterpreterWithBBCache();
    } catch (const std::exception &e) {
        std::cout << "Simulation ended with: " << e.what() << std::endl;
        std::cout << "Num of runed instructions: " << simHart->GetNumOfRunInstr() << std::endl;
    }

    if (dumpMemPath.size() != 0) {
        machine->DumpMem(dumpMemPath, pc, pc + 0x10000);
    }
}


void Simulator::AllocVirtualMemToStartProcess(std::shared_ptr<Machine::Hart> &hart) {
    // Allocate root page table
    Machine::RegValue root_page_table_paddr = machine->mem->AllocPages();
    Log(LogLevel::DEBUG, (std::stringstream() << "Root page table vaddr: "
            << std::hex << root_page_table_paddr).str());

    // set SATP register
    Machine::RegValue mmu_mode = static_cast<Machine::RegValue>(Machine::SATP_MMU_MODE::SV39);
    Machine::RegValue asid = Machine::RegValue(0) << 44;
    hart->setSpecialReg(Machine::SpecialRegs::SATP, mmu_mode | asid | (root_page_table_paddr >> 12));
    Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "SATP: 0x"
        << (mmu_mode | asid | (root_page_table_paddr >> 12))).str());

    // Allocate 0x900 pages for new process
    Machine::RegValue mem_for_program_paddr = machine->mem->AllocPages(0x900);
    Machine::RegValue access = static_cast<Machine::RegValue>(Machine::AccessType::READ) |
                      static_cast<Machine::RegValue>(Machine::AccessType::WRITE) |
                      static_cast<Machine::RegValue>(Machine::AccessType::EXECUTE);
    for (Machine::RegValue i = 0; i < 0x900; ++i) {
        Machine::RegValue vaddr = i << 12;
        Machine::RegValue paddr = mem_for_program_paddr + 0x1000 * i;
        createPTE(vaddr, paddr, access, hart);
        // Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "vaddr: 0x" << vaddr << ", paddr: 0x" << paddr).str());
    }
}


} // namespace Simulator