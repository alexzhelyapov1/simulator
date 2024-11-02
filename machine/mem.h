#include <memory>
#include <cstddef>

#ifndef MEM_H
#define MEM_H
using MemAddressType = int64_t;

class Mem
{
private:
    MemAddressType memSize;
    std::unique_ptr<std::byte[]> memArray;
public:
    Mem(MemAddressType size) : memSize(size)
    {
        memArray = std::unique_ptr<std::byte[]>(new std::byte[size]);
    }

    template <typename ValType>
    ValType loadMem(MemAddressType address);

    template <typename ValType>
    void storeMem(MemAddressType address, ValType val);
};

#endif