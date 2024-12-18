#ifndef MODULE
    #define MODULE "Simulator"
    #include "logging.h"
#endif
#include "hart.h"
#include "loader.h"
#include <memory>
#include <fstream>

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
        loader = std::make_shared<Loader::Loader>(machine, this);
    };

    void StartSimulationOnSimpleInterpreter(const std::string &filePath, const std::string &dumpMemPath = "");

    void AllocVirtualMemToStartProcess(Machine::Hart *hart);
    inline void createPTE(const RegValue vaddress, const RegValue paddress, const RegValue access, Machine::Hart *hart) const {

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

        Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "Created PTE vaddr: 0x" << vaddress
            << ", paddr: 0x" << paddress << ", access: 0x" << access).str());
    }

    RegValue LoadVirtualPage(const RegValue vaddress, Machine::Hart *hart) const {
        vm_area_struct *vma = hart->maps.FindVmAreaStructForVAddr(vaddress);

        // Page fault if not found VMA
        if (vma == nullptr) {
            Log(LogLevel::ERROR, (std::stringstream() << std::hex << "Page fault vaddr: 0x" << vaddress).str());
            hart->maps.DumpMaps();
            throw std::runtime_error("Page fault.");
        }

        // Allocate 1 zeroed page
        RegValue paddr = machine->mem->AllocPages(1);

        // If memory from file
        if (vma->vm_file != nullptr) {
            std::ifstream *file = vma->vm_file;

            int64_t start_of_vpage_in_file = vma->vm_pgoff + ((vaddress - vma->vm_start) & ~0xFFF);
            int64_t bytes_to_read = vma->vm_pgoff + vma->vm_segment_size - start_of_vpage_in_file;

            // If we need to read from file
            if (bytes_to_read > 0) {
                file->seekg(start_of_vpage_in_file);
                char *haddr = reinterpret_cast<char *>(machine->mem->GetHostAddr(paddr));
                bytes_to_read = bytes_to_read > 0xFFF ? 0xFFF : bytes_to_read;
                file->read(haddr, bytes_to_read);

                Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "Loaded from file offset: 0x"
                    << start_of_vpage_in_file << ", size: 0x"
                    << bytes_to_read << ", to paddr: 0x" << paddr).str());
            }
        }

        // Insert in page table
        createPTE(vaddress & ~0xFFF, paddr, vma->flags, hart);
#ifndef NDEBUG
        if ((paddr & 0xFFF) != 0) {
            throw std::runtime_error("Bad paddr in LoadVirtualPage.");
        }
#endif
        Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "Loaded vaddr: 0x"
            << (vaddress & ~0xFFF) << ", paddr: 0x" << paddr << ", access: 0x" << vma->flags).str());
        return paddr + vma->flags;
    }
};

} // namespace Simulator