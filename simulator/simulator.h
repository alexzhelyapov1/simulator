#ifndef MODULE
    #define MODULE "Simulator"
    #include "logging.h"
    #include <sstream>
#endif
#include "hart.h"
#include "loader.h"
#include <memory>

namespace Machine {
class Machine;
class Hart;
} // namespace Machine

namespace Loader {
class Loader;
} // namespace Loader

namespace Simulator {

class Simulator {
  private:
    std::shared_ptr<Machine::Machine> machine;
    std::shared_ptr<Loader::Loader> loader;

  public:
    Simulator(Machine::RegValue memSize) {
        machine = std::make_shared<Machine::Machine>(memSize);
        loader = std::make_shared<Loader::Loader>(machine);
    };

    void StartSimulationOnSimpleInterpreter(const std::string &filePath, const std::string &dumpMemPath = "");

    void createPTE(const RegValue vaddress, const RegValue paddress, std::shared_ptr<Machine::Hart> &hart) {
        // get VPN
        const RegValue VPN = vaddress >> 12;

        // get Offset
        const RegValue offset = vaddress & 0xFFF;

        // SV39
        const uint32_t VPN_1 = (VPN >> 18) & 0x1FF;
        const uint32_t VPN_2 = (VPN >> 9) & 0x1FF;
        const uint32_t VPN_3 = VPN & 0x1FF;

        // Insert
        RegValue *page_table_0 = nullptr;
        RegValue *page_table_1 = nullptr;
        RegValue *page_table_2 = nullptr;

        page_table_0 = reinterpret_cast<RegValue *>(machine->mem->GetHostAddr(hart->getSATP() & ~0xFFF));

        if (page_table_0[VPN_1] == 0) {
            page_table_0[VPN_1] = machine->mem->AllocPages(1);
        }
        page_table_1 = reinterpret_cast<RegValue *>(machine->mem->GetHostAddr(page_table_0[VPN_1]));

        if (page_table_1[VPN_2] == 0) {
            page_table_1[VPN_2] = machine->mem->AllocPages(1);
        }
        page_table_2 = reinterpret_cast<RegValue *>(machine->mem->GetHostAddr(page_table_1[VPN_2]));

        page_table_2[VPN_3] = paddress + 0xFFF;
    }
};

} // namespace Simulator