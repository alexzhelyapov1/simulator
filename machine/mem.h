#ifndef MODULE
    #define MODULE "Mem"
    #include "logging.h"
#endif
#include <cstddef>
#include <cstring>
#include <memory>
#include <sys/mman.h>

#ifndef MEM_H
#define MEM_H
using MemAddressType = int64_t;

namespace Machine {

using RegValue = int64_t;

class Mem {
  private:
    MemAddressType memSize;
    uint8_t *mem;
    RegValue page_allocator_pos;

  public:
    Mem(MemAddressType size) : memSize(size) {
        mem =
            static_cast<uint8_t *>(mmap(nullptr, memSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
        memset(mem, 0, memSize);
        page_allocator_pos = 0;
        Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "Base paddr: " << reinterpret_cast<uint64_t>(mem)).str());
        // if (reinterpret_cast<uint64_t>(page_allocator_pos) & 0xFFF != 0) {
        //     page_allocator_pos = reinterpret_cast<uint8_t *>((reinterpret_cast<uint64_t>(page_allocator_pos) & ~0xFFF) + 0x1000);
        // }
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

    inline RegValue AllocPages(int n_pages = 1) {
        RegValue paddr = page_allocator_pos;
        page_allocator_pos += 0x1000 * n_pages;
        return paddr;
    }

    inline uint8_t *GetHostAddr(RegValue paddr) {
        return mem + paddr;
    }
};

} // namespace Machine

#endif