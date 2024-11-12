#include <iostream>
#include <fstream>
#include <elf.h>
#include "loader.h"


namespace Loader {

#if __x86_64__ || __ppc64__
    typedef Elf64_Ehdr Elf_Ehdr;
    typedef Elf64_Phdr Elf_Phdr;
    typedef Elf64_Addr Elf_Addr;
#else
    typedef Elf32_Ehdr Elf_Ehdr;
    typedef Elf32_Phdr Elf_Phdr;
    typedef Elf32_Addr Elf_Addr;
#endif


void Loader::loadElf(const std::string &path) const {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("[ERROR]:[Loader]: Can't open file: " + path + ".");
    }


    Elf_Ehdr header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));


    if (header.e_ident[EI_MAG0] != ELFMAG0 ||
        header.e_ident[EI_MAG1] != ELFMAG1 ||
        header.e_ident[EI_MAG2] != ELFMAG2 ||
        header.e_ident[EI_MAG3] != ELFMAG3 ||
        header.e_type != ET_EXEC /* && header.e_type != ET_DYN */) {
        throw std::runtime_error("[ERROR]:[Loader]: Invalid ELF file.");
    }


    char *buffer = reinterpret_cast<char *>(calloc(0xFFFFFF, 1));
    if (!buffer) {
        throw std::runtime_error("Can't allocate buffer to read elf.");
    }

    for (int i = 0; i < header.e_phnum; ++i) {
        Elf_Phdr program_header;
        file.seekg(header.e_phoff + i * sizeof(program_header));
        file.read(reinterpret_cast<char*>(&program_header), sizeof(program_header));

        if (program_header.p_type == PT_LOAD) {
            // std::cout << std::hex << program_header.p_vaddr << " + " << program_header.p_memsz << " = " 
                // << program_header.p_vaddr + program_header.p_memsz << std::endl;
            file.seekg(program_header.p_offset);
            file.read(buffer, program_header.p_filesz);
            machine.lock()->storeMemCpy(program_header.p_vaddr, buffer, program_header.p_memsz);
        }
    }

    free(buffer);


    file.close();
}

}