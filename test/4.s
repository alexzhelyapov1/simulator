.global _start
_start:
    jal  x3, . + 16
    addi x1, x0, 10
    addi x1, x1, 10
    addi x1, x1, 10
    addi x1, x1, 10
    sret
