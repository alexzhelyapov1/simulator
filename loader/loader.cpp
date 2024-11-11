#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <sys/mman.h> // Для mmap
#include <elf.h> // Для структур ELF
#include <unistd.h> // Для sysconf

#if __x86_64__ || __ppc64__
    typedef Elf64_Ehdr Elf_Ehdr;
    typedef Elf64_Phdr Elf_Phdr;
    typedef Elf64_Addr Elf_Addr;
#else
    typedef Elf32_Ehdr Elf_Ehdr;
    typedef Elf32_Phdr Elf_Phdr;
    typedef Elf32_Addr Elf_Addr;
#endif

// Упрощенный загрузчик ELF файлов (только для x86-64 Linux)

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: loader <elf_file>\n";
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    // Чтение заголовка ELF
    Elf_Ehdr header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    // std::cout << (bool) (header.e_ident[EI_MAG0] == ELFMAG0) << std::endl
    // << (bool) (header.e_ident[EI_MAG1] == ELFMAG1) << std::endl
    // << (bool) (header.e_ident[EI_MAG2] == ELFMAG2) << std::endl
    // << (bool) (header.e_ident[EI_MAG3] == ELFMAG3) << std::endl
    // << std::endl
    // << (bool) (header.e_type == ET_EXEC) << std::endl
    // << (bool) (header.e_type == ET_DYN) << std::endl;

    // Проверка магического числа и типа файла
    if (header.e_ident[EI_MAG0] != ELFMAG0 ||
        header.e_ident[EI_MAG1] != ELFMAG1 ||
        header.e_ident[EI_MAG2] != ELFMAG2 ||
        header.e_ident[EI_MAG3] != ELFMAG3 ||
        header.e_type != ET_EXEC && header.e_type != ET_DYN) {
        std::cerr << "Invalid ELF file\n";
        return 1;
    }


    // Вычисление общего размера необходимой памяти
    size_t total_memory_size = 0;
    for (int i = 0; i < header.e_phnum; ++i) {
        Elf_Phdr program_header;
        file.seekg(header.e_phoff + i * sizeof(program_header));
        file.read(reinterpret_cast<char*>(&program_header), sizeof(program_header));

        if (program_header.p_type == PT_LOAD && (program_header.p_vaddr + program_header.p_memsz) > total_memory_size) {
            std::cout << std::hex << program_header.p_vaddr << " + " << program_header.p_memsz << " = " << program_header.p_vaddr + program_header.p_memsz << std::endl;
            total_memory_size = program_header.p_vaddr + program_header.p_memsz;
        }
    }

    std::cout << "Result total mem: " << std::hex << total_memory_size << std::endl;

    std::cout << __LINE__ << "\n";

    // Выделение памяти с помощью mmap
    void* base_address = mmap(nullptr, total_memory_size, PROT_READ | PROT_WRITE | PROT_EXEC, 
                            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (base_address == MAP_FAILED) {
        std::cerr << "Memory allocation failed\n";
        return 1;
    }

    std::cout << __LINE__ << "\n";
    // Загрузка сегментов
    for (int i = 0; i < header.e_phnum; ++i) {
        Elf_Phdr program_header;
        file.seekg(header.e_phoff + i * sizeof(program_header));
        file.read(reinterpret_cast<char*>(&program_header), sizeof(program_header));

        // std::cout << __LINE__ << "\n";
        if (program_header.p_type == PT_LOAD) {
            std::cout << "p_vaddr: " << program_header.p_vaddr << "\n";
            std::cout << "[" << header.e_phoff + i * sizeof(program_header) << " : "
                << program_header.p_filesz + header.e_phoff + i * sizeof(program_header) << "] -> ["
                << program_header.p_vaddr << " : " << program_header.p_vaddr + program_header.p_filesz << "]\n\n";
            // Копирование данных сегмента
            // std::cout << __LINE__ << "\n";
            // file.seekg(program_header.p_offset);
            // std::cout << __LINE__ << "\n";
            // file.read(reinterpret_cast<char*>(base_address) + program_header.p_vaddr, program_header.p_filesz);
            // std::cout << __LINE__ << "\n";
            // Заполнение нулями, если memsz > filesz (например, для .bss)
            // if (program_header.p_memsz > program_header.p_filesz) {
            //     std::cout << __LINE__ << "\n";
            //     std::memset(reinterpret_cast<char*>(base_address) + program_header.p_vaddr + program_header.p_filesz, 
            //                0, program_header.p_memsz - program_header.p_filesz);
            //     std::cout << __LINE__ << "\n";
            // }

            // std::cout << __LINE__ << "\n";
            //  // Установка прав доступа (очень упрощенно)
            // int prot = 0;
            // if (program_header.p_flags & PF_R) prot |= PROT_READ;
            // if (program_header.p_flags & PF_W) prot |= PROT_WRITE;
            // if (program_header.p_flags & PF_X) prot |= PROT_EXEC;


            // if (mprotect(reinterpret_cast<char*>(base_address) + program_header.p_vaddr, program_header.p_memsz, prot) == -1) {
            //     std::cerr << "mprotect failed\n";
            //     return 1;
            // }

        }
    }

    // ...  Дальнейшие действия (запуск программы, динамическая компоновка и т.д.) ...

    //  В реальном загрузчике здесь был бы вызов  кода по адресу точки входа

    std::cout << "ELF file loaded at " << base_address << "\n";

    // Освобождение памяти (в упрощенном варианте не обязательно)
    munmap(base_address, total_memory_size);

    return 0;
}