.global _start
_start:
    .equ MSTATUS, 4
    
    addi x1, x0, 2 
    csrrsi x2, MSTATUS, 2 

    csrr x2, MSTATUS
    
    addi x7, x0, 93
    sret
