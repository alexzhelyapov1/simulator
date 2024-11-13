#include <memory>
#include <cstddef>
#include <sys/mman.h>
#include <cstring>

#ifndef MEM_H
#define MEM_H
using MemAddressType = int64_t;

namespace Machine
{

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

    void DumpMemory(const std::string &outPath, const RegValue &startSize, const RegValue &endSize);

    template <typename ValType> ValType loadMem(MemAddressType address) {
        if (address < memSize - sizeof(ValType)) {
            return *(reinterpret_cast<ValType *>(mem + address));
        }
        throw std::runtime_error("ACCESS TO OUT OF RANGE MEMORY");
    }

    template <typename ValType> void storeMem(MemAddressType address, ValType val) {
        if (address < memSize - sizeof(ValType)) {
            *(reinterpret_cast<ValType *>(mem + address)) = val;
            return;
        }
        throw std::runtime_error("ACCESS TO OUT OF RANGE MEMORY");
    }

    void storeMemCpy(MemAddressType address, void *source, uint64_t size) {
        if (address < memSize - size) {
            memcpy(mem, source, size);
            return;
        }
        throw std::runtime_error("ACCESS TO OUT OF RANGE MEMORY");
    }
};

}

#endif