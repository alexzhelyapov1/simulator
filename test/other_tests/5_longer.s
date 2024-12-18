.global _start
_start:
    addi x5, x0, 1000
    addi x4, x0, 100
    mul  x5, x5, x5
    mul  x5, x5, x4
    addi x1, x0, 1
    sub  x5, x5, x1
    bne  x0, x5, . + -8
    sret
