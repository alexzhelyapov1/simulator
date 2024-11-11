#include <memory>
#include <vector>
#include "mem.h"
#ifndef MACHINE_H
#define MACHINE_H

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
public:
    Machine(RegValue memSize) 
    {
        mem = std::make_shared<Mem>(memSize);
    }    

    std::shared_ptr<Hart> CreateHart();
    std::shared_ptr<Hart> CreateHart(const RegValue& PC);

    template <typename ValType> ValType loadMem(RegValue address) { return mem->loadMem<ValType>(address); }

    template <typename ValType> void storeMem(RegValue address, ValType val) {
        return mem->storeMem(address, val);
    }
};

}

#endif