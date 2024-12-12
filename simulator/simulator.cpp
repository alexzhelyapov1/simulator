#include "simulator.h"
#include <sstream>

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

    // Alloc mem for program
    RegValue root_page_table_paddr = machine->mem->AllocPages();
    Log(LogLevel::DEBUG, (std::stringstream() << "Root page table vaddr: "
            << std::hex << root_page_table_paddr).str());
    simHart->setSATP(root_page_table_paddr + 0xFFF);
    Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "SATP: 0x" << simHart->getSATP()).str());
    
    // Allocate 0x500 pages for program
    RegValue mem_for_program_paddr = machine->mem->AllocPages(0x500);
    for (RegValue i = 0; i < 0x500; ++i) {
        RegValue vaddr = i << 12;
        RegValue paddr = mem_for_program_paddr + 0x1000 * i;
        createPTE(vaddr, paddr, simHart);
        // Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "vaddr: 0x" << vaddr << ", paddr: 0x" << paddr).str());
    }

    auto pc = loader->loadElf(filePath, simHart);
    simHart->setPC(pc);
    if (dumpMemPath.size() != 0) {
        machine->DumpMem(dumpMemPath, pc, pc + 0x10000);
    }

    try {
        simHart->RunSimpleInterpreterWithInstCache();
    } catch (const std::exception &e) {
        std::cout << "Simulation ended with: " << e.what() << std::endl;
        std::cout << "Num of runed instructions: " << simHart->GetNumOfRunInstr() << std::endl;
    }

    if (dumpMemPath.size() != 0) {
        machine->DumpMem(dumpMemPath, pc, pc + 0x10000);
    }
}

} // namespace Simulator