#include "mem.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace Machine {

static std::string int_to_hex(uint8_t b) {
    std::stringstream ss;
    ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase
       << static_cast<unsigned int>(static_cast<uint8_t>(b)) << std::dec;
    return ss.str();
}

void Mem::DumpMemory(const std::string &outPath, const int64_t &startAddress, const int64_t &endAddress) {
    std::ofstream outFile(outPath, std::ios::out | std::ios::trunc);

    if (!outFile.is_open()) {
        std::cout << "FAILED OPEN OUT FILE: \"" << outPath << "\"" << std::endl;
    }

    for (int64_t i = startAddress; i < endAddress && i < memSize; i++) {
        outFile << "[" << i << "]: " << int_to_hex((mem[i])) << " ";
        if ((i - startAddress) % 4 == 3) {
            outFile << std::endl;
        }
    }

    outFile.close();
}

} // namespace Machine