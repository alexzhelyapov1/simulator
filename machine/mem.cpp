#include "mem.h"
#include <stdexcept>

template <typename ValType>
ValType Mem::loadMem(MemAddressType address)
{
    if(address < memSize) {
        return *(reinterpret_cast<ValType *>(memArray[address]));
    }
    throw std::runtime_error("ACCESS TO OUT OF RANGE MEMORY");
}

template <typename ValType>
void Mem::storeMem(MemAddressType address, ValType val)
{
    if(address < memSize) {
        *(reinterpret_cast<ValType *>(memArray[address])) = val;
        return;
    }
    throw std::runtime_error("ACCESS TO OUT OF RANGE MEMORY");
}