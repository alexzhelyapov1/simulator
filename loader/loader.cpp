#include "loader.h"
#include <elf.h>
#include <fstream>
#include <iostream>
#include "vm_area_struct.h"
#include "simulator.h"

#include "hart.h"


namespace Loader {

typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Addr Elf_Addr;

uint64_t Loader::loadElf(const std::string &path, Machine::Hart *hart) const {
    std::ifstream *file = new std::ifstream(path, std::ios::binary);
    if (!file->is_open()) {
        throw std::runtime_error("[ERROR]:[Loader]: Can't open file: " + path + ".");
    }

    Elf_Ehdr header;
    file->read(reinterpret_cast<char *>(&header), sizeof(header));

    if (header.e_ident[EI_MAG0] != ELFMAG0 || header.e_ident[EI_MAG1] != ELFMAG1 ||
        header.e_ident[EI_MAG2] != ELFMAG2 || header.e_ident[EI_MAG3] != ELFMAG3 ||
        header.e_type != ET_EXEC /* && header.e_type != ET_DYN */) {
        throw std::runtime_error("[ERROR]:[Loader]: Invalid ELF file.");
    }

    for (int i = 0; i < header.e_phnum; ++i) {
        Elf_Phdr program_header;
        file->seekg(header.e_phoff + i * sizeof(program_header));
        file->read(reinterpret_cast<char *>(&program_header), sizeof(program_header));

        if (program_header.p_type == PT_LOAD) {
            hart->maps.MMAP(
                program_header.p_vaddr,
                program_header.p_memsz,
                MemAccessType::READ | MemAccessType::WRITE | MemAccessType::EXEC,
                file,
                program_header.p_offset,
                program_header.p_filesz);

            for (int i = 0; i < (program_header.p_memsz + 0xFFF) / 0x1000; ++i) {
                simulator->createPTE(
                    program_header.p_vaddr + 0x1000 * i,
                    0,
                    MemAccessType::READ | MemAccessType::WRITE | MemAccessType::EXEC | MemAccessType::INVALID,
                    hart);
            }

            // hart->maps.DumpMaps();
        }
    }

    // file->close();

    Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "Entry point (va): 0x" << header.e_entry).str());

    return header.e_entry;
}

} // namespace Loader