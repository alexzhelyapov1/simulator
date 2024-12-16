# simulator
Risc-V simulator

### How to run
```
ninja Simulator
./build/Simulator <path-to-elf-file>
```

### How to get elf file
```
compile-elf.sh <input-file> <output-dir> <output-filename>
```
Example:
```
compile-elf.sh ./test/2.s ./build myelf
```

We have integration testing. You can run code below to check if all tests work:
```
./test_runner.sh
```


#### Suggestions:
1) Create your own brunch to develop your own module.
2) Master only for releases but there are no lock to push in master.
3) Use ninja, it's much more faster, than `Unix Makefiles`.
4) If you don't like module name, you can rename it, but please fix main `CMakeLists.txt` with new correct path.
5) Please add all requirements you're isntalling, to [script](./install_third_party.sh).


## TODO (global project):
- [ ] [MARIA + NIKITA]: Confirm heart API for instructions map generation.  
Basic instructions like GetReg, SysCall and others.  
- [ ] [ALEKSEY + NIKITA]: Confirm machine API for memory access.
- [x] [ALEKSEY]: ninja target `clang-force-format`.


## Questions:
- [ALEKSEY]: Do we need to implement SysCalls like printf and others.


## Notes:
- [NIKITA]: Release instructions decoding in parallel.


## Distribution of responsibilities:
Basic version:
- Instructions map generation - [MARIA]
- Machine - [NIKITA]
- Loader - [ALEKSEY]
