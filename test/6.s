.global _start
_start:
    addi x4, x0, 10
    addiw x5, x0, -5


    mul x1, x0, x1
    mulw x2, x5, x5

    mulh x3, x1, x1  
    mulhu x4, x1, x1 
    mulhsu x5, x2, x2

    div x2, x5, x5  
    divu x3, x5, x2 
    rem x5, x2, x3  
    remu x1, x5, x3 
    div x2, x1, x3
    mret


   
