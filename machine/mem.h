#include <cstddef>
#include <cstring>
#include <memory>
#include <sys/mman.h>

#ifndef MEM_H
#define MEM_H
using MemAddressType = int64_t;

namespace Machine {

class Mem {
  private:
    MemAddressType memSize;
    uint8_t *mem;

  public:
    Mem(MemAddressType size) : memSize(size) {
        mem =
            static_cast<uint8_t *>(mmap(nullptr, memSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    }

    void DumpMemory(const std::string &outPath, const int64_t &startAddress, const int64_t &endAddress);

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
        if (size + address >= memSize) {
            throw std::runtime_error("ACCESS TO OUT OF RANGE MEMORY");
        }
        for (int64_t i = 0; i < size; i++) {
            mem[i + address] = ((uint8_t *)(source))[i];
        }
    }
};

} // namespace Machine

#endif