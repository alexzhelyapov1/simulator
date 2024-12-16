.global _start

_start:
  addi  x5, x0, 10
  addi  x6, x0, 20
  slli  x7, x5, 2
  srli  x7, x7, 2
  mul   x7, x7, x7
  add   x7, x7, x5
  add   x7, x7, x5
  div   x7, x7, x5
  sret
  