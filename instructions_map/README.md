## TODO (instructions map generation):
We use RV32I + RV64I + RV64M (to be able to use multiplication and division)

#### How to run it now, you'll get generated_functions.cpp that can be used for decoder module
ruby process_instrs.rb

#### Functions in the decoder that must be implemented and used in instruction map
getReg - return int64_t
setReg 

storeMem
loadMem
getPc()
setPc()

###### Instructions that have not yet been implemented:
[X] Check S-format imm -> Add functions getImm - that return 32bit imm

[X] BEQ BNE BLT BGE BLTU BGEU 

FENCE FENCE.I - not yet implemented
CSRRW CSRRS CSRRC CSRRWI CSRRSI CSRRCI 
ECALL EBREAK
URET SRET MRET
WFI SFENCE.VMA

[ ] Assembly - ? (For tests)

Done + checked:
[X] S-format
[X] U-format
[X] I
[X] R
[X] J - need to check JALR (& ~1 is necessary? + sext)
[X] B
[ ] M