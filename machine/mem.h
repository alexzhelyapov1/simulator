#include <memory>
#include <cstddef>
#include <sys/mman.h>

#ifndef MEM_H
#define MEM_H
using MemAddressType = int64_t;

class Mem
{
private:
    MemAddressType memSize;
    std::byte* mem;
public:
    Mem(MemAddressType size) : memSize(size)
    { 
        mem = static_cast<std::byte *>(mmap(nullptr, memSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    }

    template <typename ValType>
    ValType loadMem(MemAddressType address);

    template <typename ValType>
    void storeMem(MemAddressType address, ValType val);
};

#endif