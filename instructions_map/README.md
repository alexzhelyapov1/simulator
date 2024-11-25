## TODO (instructions map generation):
We use RV32I + RV64I + RV64M (to be able to use multiplication and division)

#### How to run it now, you'll get generated_functions.cpp that can be used for decoder module
ruby process_instrs.rb

#### Functions in the decoder that must be implemented and used in instruction map
getReg - return int64_t
setReg 

storeMem
loadMem
getPC()
setPc()

###### Instructions that have not yet been implemented:
FENCE FENCE.I - not yet implemented
CSRRW CSRRS CSRRC CSRRWI CSRRSI CSRRCI 
ECALL EBREAK
URET SRET MRET
WFI SFENCE.VMA

[ ] Fix div and same instrs