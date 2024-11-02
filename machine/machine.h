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
public:
    Machine() {}
    ~Machine() {}

    

    std::shared_ptr<Hart> CreateHart();

    template <typename ValType>
    ValType loadMem(RegValue address);
    
    template <typename ValType>
    void storeMem(RegValue address, ValType val);
};

}

#endif