#include "mem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "hart.h"
#include <iomanip>

namespace Machine
{

static std::string int_to_hex(std::byte b)
{
    std::stringstream ss;
    ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << static_cast<unsigned int>(static_cast<uint8_t>(b));
    return ss.str();
}

void Mem::DumpMemory(const std::string &outPath, const RegValue &startSize, const RegValue &endSize)
{
    std::ofstream outFile(outPath, std::ios::out | std::ios::trunc);

    if (!outFile.is_open()) {
        std::cout << "FAILED OPEN OUT FILE: \"" << outPath << "\"" << std::endl;
    }

    for(RegValue i = startSize; i < endSize && i < memSize; i++)
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