#include "loader.h"
#include <elf.h>
#include <fstream>
#include <iostream>

#include "hart.h"


namespace Loader {

typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Addr Elf_Addr;

uint64_t Loader::loadElf(const std::string &path, std::shared_ptr<Machine::Hart> &hart) const {
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
            file.seekg(program_header.p_offset);
            file.read(buffer, program_header.p_filesz);
            // copy p_filesz bytes, other p_memsz - p_filesz bytes are zero
            Machine::RegValue paddr = hart->MMU(program_header.p_vaddr, Machine::AccessType::WRITE);
            machine.lock()->storeMemCpy(paddr, buffer, program_header.p_filesz);

            Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "Loaded segment:" << "vaddr: 0x" 
                << program_header.p_vaddr << ", paddr: 0x" << paddr << ", size: 0x" << program_header.p_filesz).str());
        }
    }

    free(buffer);

    file.close();

    Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "Entry point (va): 0x" << header.e_entry).str());

    return header.e_entry;
}

} // namespace Loader