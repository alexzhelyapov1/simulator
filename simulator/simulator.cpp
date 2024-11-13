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

    try {
        simHart->RunSimpleInterpreterWithInstCache();
    } catch (const std::exception &e) {
        std::cout << "Simulation ended with: " << e.what() << std::endl;
    }
}

} // namespace Simulator