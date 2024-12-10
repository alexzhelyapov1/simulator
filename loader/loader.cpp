#include "loader.h"
#include <elf.h>
#include <fstream>
#include <iostream>

#define MODULE "Loader"
#include "logging.h"

namespace Loader {

typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Addr Elf_Addr;

uint64_t Loader::loadElf(const std::string &path) const {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("[ERROR]:[Loader]: Can't open file: " + path + ".");
    }

    Elf_Ehdr header;
    file.read(reinterpret_cast<char *>(&header), sizeof(header));

    if (header.e_ident[EI_MAG0] != ELFMAG0 || header.e_ident[EI_MAG1] != ELFMAG1 ||
        header.e_ident[EI_MAG2] != ELFMAG2 || header.e_ident[EI_MAG3] != ELFMAG3 ||
        header.e_type != ET_EXEC /* && header.e_type != ET_DYN */) {
        throw std::runtime_error("[ERROR]:[Loader]: Invalid ELF file.");
    }

    uint64_t buffer_size = 0;
    for (int i = 0; i < header.e_phnum; ++i) {
        Elf_Phdr program_header;
        file.seekg(header.e_phoff + i * sizeof(program_header));
        file.read(reinterpret_cast<char *>(&program_header), sizeof(program_header));

        if (program_header.p_type == PT_LOAD && program_header.p_filesz > buffer_size) {
            buffer_size = program_header.p_filesz;
        }
    }

    Log(LogLevel::DEBUG, "Buffer size: " + std::to_string(buffer_size));

    char *buffer = reinterpret_cast<char *>(calloc(buffer_size, 1));
    if (!buffer) {
        throw std::runtime_error("Can't allocate buffer to read elf.");
    }

    for (int i = 0; i < header.e_phnum; ++i) {
        Elf_Phdr program_header;
        file.seekg(header.e_phoff + i * sizeof(program_header));
        file.read(reinterpret_cast<char *>(&program_header), sizeof(program_header));

        if (program_header.p_type == PT_LOAD) {
            // std::cout << std::hex << program_header.p_vaddr << " + " << program_header.p_memsz << " = "
            // << program_header.p_vaddr + program_header.p_memsz << std::endl;
            file.seekg(program_header.p_offset);
            file.read(buffer, program_header.p_filesz);
            // copy p_filesz bytes, other p_memsz - p_filesz bytes are zero
            machine.lock()->storeMemCpy(program_header.p_vaddr, buffer, program_header.p_filesz);
            Log(LogLevel::DEBUG, "Load segment: VADDR = " + std::to_string(program_header.p_vaddr) + ", size = " +
                std::to_string(program_header.p_filesz) + " bytes.");
        }
    }

    free(buffer);

    file.close();

    Log(LogLevel::DEBUG, "Entry point = " + std::to_string(header.e_entry));
    return header.e_entry;
}

} // namespace Loader