#include "simulator.h"

namespace Simulator {

void Simulator::StartSimulationOnSimpleInterpreter(const std::string &filePath) {
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

    // TODO: Give loader pages for loadElf
    simHart->setPC(loader->loadElf(filePath));
    // void *buff = new Machine::Word(0xCD023);
    // machine->storeMemCpy(0x10000, buff, 4U);
    // std::cout << "load Mem: " << machine->loadMem<Machine::Word>(0x10000) << std::endl;
    machine->DumpMem("./build/dump.txt", 0x10000, 0x11000);

    try {
        simHart->RunSimpleInterpreterWithInstCache();
    } catch (const std::exception &e) {
        std::cout << "Simulation ended with: " << e.what() << std::endl;
    }
}

} // namespace Simulator