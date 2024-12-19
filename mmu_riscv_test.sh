SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD="$SCRIPT_DIR/build"

help() {
    echo -e "
--------------------------------------------
Usage: bash $0 (Debug|Release)
Example: bash $0 Release
Description:
Run 2 test files: 'mmu.cpp' and 'mmu_page_fault.cpp'
    - Debug will dump simulator logs in files.
    - Release runs without logs.
You are able to see 'page fault' in any mode.

Note: 'EXCEPTION RETURN FROM HART: RET FROM MAIN' is correct ending of the programm.
--------------------------------------------
"
}

if (( $# != 1 )); then
    help
    exit -1
fi

if [ "$1" == "Release" ]; then
    echo "Running Release mode."
elif [ "$1" == "Debug" ]; then
    echo "Running Debug mode."
else
    help
    exit -1
fi

if [[ ! -d build ]]; then
    cmake -GNinja -DCMAKE_BUILD_TYPE=$1 -S "$SCRIPT_DIR" -B "$BUILD"
    cmake --build "$BUILD" --parallel -j$(nproc)
fi

riscv64-linux-gnu-gcc -march=rv64im -nostdlib -mabi=lp64 -static "$SCRIPT_DIR/test/mmu.cpp" -o "$BUILD/mmu.elf"
riscv64-linux-gnu-gcc -march=rv64im -nostdlib -mabi=lp64 -static "$SCRIPT_DIR/test/mmu_page_fault.cpp" -o "$BUILD/mmu_page_fault.elf"

cd $BUILD
ninja Simulator


if [ "$1" == "Release" ]; then
    echo Running: "$BUILD/Simulator" "$BUILD/mmu.elf"
    "$BUILD/Simulator" "$BUILD/mmu.elf"

    echo Running: "$BUILD/Simulator" "$BUILD/mmu_page_fault.elf"
    "$BUILD/Simulator" "$BUILD/mmu_page_fault.elf"

elif [ "$1" == "Debug" ]; then
    echo Running: "$BUILD/Simulator" "$BUILD/mmu.elf" \> $BUILD/mmu.log
    "$BUILD/Simulator" "$BUILD/mmu.elf" > $BUILD/mmu.log

    echo Running: "$BUILD/Simulator" "$BUILD/mmu_page_fault.elf" \> $BUILD/mmu_page_fault.log
    "$BUILD/Simulator" "$BUILD/mmu_page_fault.elf" > $BUILD/mmu_page_fault.log
    echo -e "
----------------------------------
See test logs:
$BUILD/mmu_test.log
$BUILD/mmu_page_fault.log
"
fi
