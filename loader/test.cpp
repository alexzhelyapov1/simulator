#include <elf.h>
#include <fstream>
#include <iostream>

#if __x86_64__ || __ppc64__
typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Addr Elf_Addr;
#else
typedef Elf32_Ehdr Elf_Ehdr;
typedef Elf32_Phdr Elf_Phdr;
typedef Elf32_Addr Elf_Addr;
#endif

int main() {
    std::string path = "/home/alex/mipt/simulator/build/a.out";
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("[ERROR]:[Loader]: Can't open file: " + path + ".");
    }

    Elf_Ehdr header;
    file.read(reinterpret_cast<char *>(&header), sizeof(header));

    std::cout << std::hex << header.e_entry << std::dec << std::endl;

    file.close();
 }