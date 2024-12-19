typedef unsigned long long ulong;
#define PADDR(ptr) (ulong)(ptr)
#define PTR(paddr) (ulong *)(paddr)

// 3 for SV39, 4 for SV48
#define SV 3

#define CREATE_PTE                                                                                                     \
    current_page_table = root_page_table;                                                                              \
    VPN = vaddr >> 12;                                                                                                 \
    for (int i = 0; i < SV - 1; ++i) {                                                                                 \
        ulong VPN_i = (VPN >> (9 * (SV - 1 - i))) & ((1 << 9) - 1);                                                    \
        if (current_page_table[VPN_i] == 0) {                                                                          \
            current_page_table[VPN_i] = available_paddr;                                                               \
            available_paddr += 0x1000;                                                                                 \
        }                                                                                                              \
        current_page_table = PTR(current_page_table[VPN_i]);                                                           \
    }                                                                                                                  \
    current_page_table[VPN & ((1 << 9) - 1)] = (paddr & ~0xFFF) + 0xF;

int main() {
    ulong *root_page_table = 0;
    ulong available_paddr = 0x1000;
    ulong vaddr, paddr, VPN;
    ulong *current_page_table;

    // Segment .text MMU vaddr: 0x10000 => paddr: 0x10000
    vaddr = 0x10000;
    paddr = 0x10000;
    CREATE_PTE

    // Stack MMU vaddr: 0xFF000 => 0xFF000
    vaddr = 0xFF000;
    paddr = 0xFF000;
    CREATE_PTE

    // Turn on address tranlation
    asm("csrrw x0, satp, %[value]" ::[value] "r"(ulong(SV) << 60));

    int sum = 0;
    for (int i = 0; i < 100; ++i) {
        sum += sum;
    }
    int *valid_ptr = &sum;
    *valid_ptr += 1;
    int *invalid_ptr = (int *)(0x13000);
    *invalid_ptr += 1;
}

// riscv64-linux-gnu-gcc -march=rv64im -nostdlib -mabi=lp64 -static ../test/mmu.cpp