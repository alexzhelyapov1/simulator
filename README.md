# simulator
Risc-V simulator


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
