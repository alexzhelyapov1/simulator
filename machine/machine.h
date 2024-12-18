#ifndef MODULE
    #define MODULE "Machine"
    #include "logging.h"
#endif
#include "mem.h"
#include <memory>
#include <vector>
#ifndef MACHINE_H
#define MACHINE_H

namespace Loader {
    class Loader;
}

namespace Simulator {
    class Simulator;
}

namespace Machine {

using RegValue = int64_t;
using RegId = int8_t;
using Word = int32_t;
using uWord = uint32_t;

class Hart;

class Machine {
  private:
    std::vector<std::shared_ptr<Hart>> harts;
    std::shared_ptr<Loader::Loader> loader;

  public:
    std::shared_ptr<Mem> mem;
    Machine(RegValue memSize) { mem = std::make_shared<Mem>(memSize); }

    std::shared_ptr<Hart> CreateHart(const Simulator::Simulator *simulator);
    std::shared_ptr<Hart> CreateHart(const RegValue &PC, const Simulator::Simulator *simulator);

    std::vector<std::shared_ptr<Hart>> &GetHarts() { return harts; }

    template <typename ValType> ValType loadMem(RegValue address) { return mem->loadMem<ValType>(address); }

    template <typename ValType> void storeMem(RegValue address, ValType val) { return mem->storeMem(address, val); }

    void DumpMem(const std::string &outPath, const RegValue &startAddress, const RegValue &endAddress) {
        mem->DumpMemory(outPath, startAddress, endAddress);
    }

    void storeMemCpy(MemAddressType address, void *source, uint64_t size) { mem->storeMemCpy(address, source, size); }
};

} // namespace Machine

#endif