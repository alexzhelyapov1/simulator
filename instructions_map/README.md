## TODO (instructions map generation):
We use RV32I + RV64I + RV64M (to be able to use multiplication and division)

#### How to run it now, you'll get generated_functions.cpp that can be used for decoder module
ruby process_instrs.rb

#### Functions in the decoder that must be implemented and used in instruction map
get_reg
set_reg
store_mem
load_mem
get_pc()
set_pc()

###### Instructions that have not yet been implemented:
BEQ BNE BLT BGE BLTU BGEU
FENCE FENCE.I
CSRRW CSRRS CSRRC CSRRWI CSRRSI CSRRCI 
ECALL EBREAK
URET SRET MRET
WFI SFENCE.VMA

###### 64I:
ADDIW
SLLIW SRLIW SRAIW
ADDW SUBW
SLLW SRLW SRAW
LWU LD
SD

###### 32M:
MUL MULH MULHSU 
DIV DIVU
REM REMU

###### 64M:
MULW 
DIVW DIVUW 
REMW REMUW



