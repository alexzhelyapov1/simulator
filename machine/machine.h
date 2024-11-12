#include <memory>
#include <vector>
#include "mem.h"
#ifndef MACHINE_H
#define MACHINE_H

namespace Loader {
    class Loader;
}

namespace Machine 
{

using RegValue = int64_t;
using RegId = int8_t;
using Word = int32_t;

class Hart;

class Machine : public std::enable_shared_from_this<Machine>
{
private:
    std::vector<std::shared_ptr<Hart>> harts;
    std::shared_ptr<Mem> mem;
    std::shared_ptr<Loader::Loader> loader;
public:
    Machine(RegValue memSize) 
    {
        mem = std::make_shared<Mem>(memSize);
    }

    std::shared_ptr<Hart> CreateHart();
    std::shared_ptr<Hart> CreateHart(const RegValue& PC);

    std::vector<std::shared_ptr<Hart>> &GetHarts() {return harts;}

    template <typename ValType> ValType loadMem(RegValue address) { return mem->loadMem<ValType>(address); }

    template <typename ValType> void storeMem(RegValue address, ValType val) {
        return mem->storeMem(address, val);
    }

    void storeMemCpy(MemAddressType address, void *source, uint64_t size) { mem->storeMemCpy(address, source, size); }
};

}

#endif