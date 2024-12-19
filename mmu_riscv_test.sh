cd $(dirname $0)
mkdir -p build
cd build
riscv64-linux-gnu-gcc -nostdlib -march=rv64i -mabi=lp64 --static -Wl,-emain ../test/mmu.cpp -o mmu.elf
riscv64-linux-gnu-objdump -M no-aliases -M numeric -d mmu.elf > mmu.objdump
ninja Simulator
ninja Simulator
./Simulator ./mmu.elf