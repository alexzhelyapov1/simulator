#pragma once
#ifndef MODULE
    #define MODULE "VMA"
    #include "logging.h"
#endif
#include <cstdint>
#include <fstream>


enum class MemAccessType {
    READ = 1,
    WRITE = 1 << 1,
    EXEC = 1 << 2,
    INVALID = 1 << 3,
};

constexpr int64_t operator|(MemAccessType a, MemAccessType b) {
    return static_cast<int64_t>(a) | static_cast<int64_t>(b);
}

constexpr int64_t operator|(int64_t a, MemAccessType b) {
    return a | static_cast<int64_t>(b);
}


struct mm_struct;


struct vm_area_struct {
    vm_area_struct(int64_t vaddr, size_t length, int64_t access, std::ifstream *file, int64_t offset,
        int64_t segment_size):
            vm_start(vaddr),
            vm_end(vaddr + length),
            vm_file(file),
            vm_pgoff(offset),
            vm_segment_size(segment_size),
            flags(access){}

    int64_t vm_start = 0;
    int64_t vm_end = 0;
    vm_area_struct *vm_next = nullptr;
    vm_area_struct *vm_prev = nullptr;
    mm_struct *vm_mm = nullptr;
    std::ifstream *vm_file = nullptr;
    int64_t vm_pgoff = 0; // я не понял каким образом нам достаточно хранить только номер страницы, поэтому я пока сделал простое смещение в файле
    int64_t vm_segment_size = 0; // добавил чтобы понимать сколько грузить из файла, а сколько нулями
    int64_t flags = 0; // access
    // int64_t vm_ops;
};


struct mm_struct {
    ~mm_struct() {
        if (vm_areas == nullptr) {
            return;
        }

        vm_area_struct *current = vm_areas;
        while (current->vm_next != nullptr) {
            vm_area_struct *next = current->vm_next;
            if (current->vm_file->is_open()) {
                current->vm_file->close();
            }
            delete current;
            current = next;
        }
        delete current;
    }

    vm_area_struct *vm_areas = nullptr;

    vm_area_struct *FindVmAreaStructForVAddr(const int64_t vaddr) {
        vm_area_struct *current = vm_areas;
        while (current != nullptr) {
            if (vaddr >= current->vm_start && vaddr < current->vm_end) {
                return current;
            }
            current = current->vm_next;
        }
        return nullptr;
    }

    void InsertVmAreaStruct(vm_area_struct * const new_vma) {
        new_vma->vm_mm = this;

        if (vm_areas == nullptr) {
            vm_areas = new_vma;
            return;
        }

        vm_area_struct *current = vm_areas;
        while (current->vm_next != nullptr) {
            if (new_vma->vm_start >= current->vm_end
                && new_vma->vm_end <= current->vm_next->vm_start) {
                    new_vma->vm_prev = current;
                    new_vma->vm_next = current->vm_next;
                    current->vm_next->vm_prev = new_vma;
                    current->vm_next = new_vma;
                    Log(LogLevel::WARNING, "Not tail insert.");
                    return;
            }

            current = current->vm_next;
        }

        // Tail insert
        current->vm_next = new_vma;
        new_vma->vm_prev = current;
    }

    void MMAP(int64_t vaddr, size_t length, int64_t access, std::ifstream *file, int64_t offset, int64_t segment_size) {
        vm_area_struct *new_vma = new vm_area_struct(
            // Version with align vaddr
            // vaddr & ~0xFFF,
            // (length & 0xFFF) == 0 ? length : ((length + 0x1000) & ~0xFFF),
            vaddr,
            length,
            access,
            file,
            offset,
            segment_size);
        InsertVmAreaStruct(new_vma);
    }

    void DumpMaps() {
        vm_area_struct *current = vm_areas;
        Log(LogLevel::DEBUG, "VMA dump begin -----------------------");
        while (current != nullptr) {
            std::string access_str = "---";
            if ((current->flags & PROT_READ) != 0) {
                access_str[0] = 'R';
            }
            if ((current->flags & PROT_WRITE) != 0) {
                access_str[1] = 'W';
            }
            if ((current->flags & PROT_EXEC) != 0) {
                access_str[2] = 'X';
            }

            std::string from = "zeroed pages";
            if (current->vm_file != nullptr) {
                from = (std::stringstream() << std::hex << "file offset: 0x" << current->vm_pgoff
                << ", segment size: " << current->vm_segment_size).str();
            }

            Log(LogLevel::DEBUG, (std::stringstream() << std::hex << "0x" << current->vm_start << "-0x"
                << current->vm_end << " | " << access_str << " | " << from).str());

            current = current->vm_next;
        }
        Log(LogLevel::DEBUG, "VMA dump end -------------------------");
    }
};

