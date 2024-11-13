.global _start
_start:
    addi x5, x0, 65
    addi x4, x0, 1000
    mul  x5, x5, x4
    addi x5, x5, 656
    addi x5, x5, 1000
    addi x3, x0, 42
    mul  x3, x3, x4
    sw x3, 0(x5)
    lw x10, 0(x5)
    sret
