#include "mem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace Machine
{

static std::string int_to_hex(uint8_t b)
{
    std::stringstream ss;
    ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << static_cast<unsigned int>(static_cast<uint8_t>(b)) << std::dec;
    return ss.str();
}

void Mem::DumpMemory(const std::string &outPath, const int64_t &startSize, const int64_t &endSize)
{
    std::ofstream outFile(outPath, std::ios::out | std::ios::trunc);

    if (!outFile.is_open()) {
        std::cout << "FAILED OPEN OUT FILE: \"" << outPath << "\"" << std::endl;
    }

    for(int64_t i = startSize; i < endSize && i < memSize; i++)
    {
        outFile << int_to_hex((mem[i])) << " ";
        if(i % 10 == 9)
        {
            outFile << std::endl;
        }
    }

    outFile.close();

}

}