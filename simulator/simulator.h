#ifndef MODULE
    #define MODULE "Simulator"
    #include "logging.h"
#endif
#include "hart.h"
#include "loader.h"
#include <memory>
#include <dlfcn.h>

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

    void StartSimulationOnSimpleInterpreter(const std::string &filePath, const std::string &PluginPath = "", const std::string &dumpMemPath = "");

    void AllocVirtualMemToStartProcess(std::shared_ptr<Machine::Hart> &hart);
    inline void createPTE(const RegValue vaddress, const RegValue paddress, const RegValue access, std::shared_ptr<Machine::Hart> &hart) {

        static int nested_transitions = (static_cast<int64_t>(hart->getSatpMmuMode()) >> 60) - 1;
        RegValue *current_page_table = reinterpret_cast<RegValue *>(machine->mem->GetHostAddr(hart->getRootPageTablePaddr()));

        const RegValue VPN = vaddress >> 12;
        for (int i = 0; i < nested_transitions; i++) {
            RegValue VPN_i = (VPN >> 9 * (nested_transitions - i)) & ((1 << 9) - 1);
            if (current_page_table[VPN_i] == 0) {
                current_page_table[VPN_i] = machine->mem->AllocPages(1);
            }
            current_page_table = reinterpret_cast<RegValue *>(machine->mem->GetHostAddr(current_page_table[VPN_i]));
        }

        current_page_table[VPN & ((1 << 9) - 1)] = paddress + access;
    }
};

} // namespace Simulator