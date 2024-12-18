#include "simulator.h"
#include "vm_area_struct.h"

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
        simHart = machine->CreateHart(this);
    }

    AllocVirtualMemToStartProcess(simHart.get());
    auto pc = loader->loadElf(filePath, simHart.get());
    simHart->maps.DumpMaps();
    simHart->setPC(pc);
    if (dumpMemPath.size() != 0) {
        machine->DumpMem(dumpMemPath, pc, pc + 0x10000);
    }

    try {
        simHart->RunInterpreterWithBBCache();
    } catch (const std::exception &e) {
#ifdef SIM_TIME
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - simHart->getStartSimTime());
        std::cout << "Sim duration: " << duration.count() << "(us)" << std::endl;
#endif
        std::cout << "Simulation ended with: " << e.what() << std::endl;
        std::cout << "Num of runed instructions: " << simHart->GetNumOfRunInstr() << std::endl;
    }

    if (dumpMemPath.size() != 0) {
        machine->DumpMem(dumpMemPath, pc, pc + 0x10000);
    }
}


void Simulator::AllocVirtualMemToStartProcess(Machine::Hart *hart) {
    // Allocate root page table
    RegValue root_page_table_paddr = machine->mem->AllocPages();
    Log(LogLevel::DEBUG, (std::stringstream() << "Root page table vaddr: "
            << std::hex << root_page_table_paddr).str());

    // set SATP register
    RegValue mmu_mode = static_cast<RegValue>(Machine::SATP_MMU_MODE::SV39);
    RegValue asid = RegValue(0) << 44;
    hart->setSatp(mmu_mode | asid | (root_page_table_paddr >> 12));
    Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "SATP: 0x"
        << (mmu_mode | asid | (root_page_table_paddr >> 12))).str());

    // Allocate stack mem 0x100000-0x1003FFF
    hart->maps.MMAP(
        0x100000,
        0x10000,
        MemAccessType::READ | MemAccessType::WRITE,
        nullptr,
        0,
        0);
    for (int i = 0; i < 10; ++i) {
        createPTE(
            0x100000 + i * 0x1000, // vaddr
            0,
            MemAccessType::READ | MemAccessType::WRITE | MemAccessType::EXEC | MemAccessType::INVALID,
            hart);
    }
    hart->setReg(2, 0x108FFF);
}


} // namespace Simulator