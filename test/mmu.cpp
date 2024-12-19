void init_mmu() {
    // set MMU mode: 0x3 (SV39) and root page table paddr: 0x0
    unsigned long long satp = 3;
    satp <<= 60;
    asm("csrrw x0, satp, %[value]" ::[value] "r"(satp));
}

int fibonacci_recursive(int n) {
    if (n <= 1)
        return 1;
    return fibonacci_recursive(n - 1) + fibonacci_recursive(n - 2);
}

int main() {
    init_mmu();
    return fibonacci_recursive(3);
}

// riscv64-linux-gnu-gcc -nostdlib -march=rv64i -mabi=lp64 --static -Wl,-emain ../test/mmu.cpp
// riscv64-linux-gnu-objdump -M no-aliases -M numeric -d