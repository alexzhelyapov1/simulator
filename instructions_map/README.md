## TODO (instructions map generation):
We use RV32I + RV64I + RV64M (to be able to use multiplication and division)

#### How to run it now, you'll get generated_functions.cpp that can be used for decoder module
ruby process_instrs.rb

#### Functions in the decoder that must be implemented and used in instruction map
get_reg - по умолчанию возвращают int64_t
set_reg 

store_mem
load_mem
get_pc()
set_pc()

###### Instructions that have not yet been implemented:
BEQ BNE BLT BGE BLTU BGEU - требует уточнений, надо аккуратно собирать imm
FENCE FENCE.I - не ясно как реализовать, нужно ли это для базовой версии симулятора?
CSRRW CSRRS CSRRC CSRRWI CSRRSI CSRRCI 
ECALL EBREAK
URET SRET MRET
WFI SFENCE.VMA