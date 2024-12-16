.global _start
_start:
    jal main
    ; jal exit
main:
# %bb.0:
	addi sp, sp, -480
	sd	x1, 472(sp)
	sd	s0, 464(sp)
	addi	s0, sp, 480
	li	x0, 0
	sw	x0, -20(s0)
.LBB4_7:
	la	x0, .L__const.main.compare
	sd	x0, -464(s0)
    ld  x0, 0(x0)
    sd  x0, -456(s0)
    la  x0, .L__const.main.compare+8
    ld  x0, 0(x0)
    sd  x0, -448(s0)
    la  x0, .L__const.main.compare+16
    ld  x0, 0(x0)
    sd  x0, -440(s0)
    la  x0, .L__const.main.compare+24
    ld  x0, 0(x0)
    sd  x0, -432(s0)
	li	x0, 1
	sw	x0, -424(s0)
	jal	.LBB4_1
.LBB4_1:                                # =>This Inner Loop Header: Depth=1
	lw	x1, -424(s0)
	li	x0, 8
	blt	x0, x1, .LBB4_6
	jal	.LBB4_2
.LBB4_2:                                #   in Loop: Header=BB4_1 Depth=1
.LBB4_8:                                #   in Loop: Header=BB4_1 Depth=1
    la	x0, solution_count
    sw  zero, 0(x0)
	lw	x2, -424(s0)
	addi	x0, s0, -420
	; call	backtrack
    jal backtrack
    la  x1, solution_count
    lw  x0, 0(x1)
	lw	x1, -424(s0)
	addi	x1, x1, -1
	slli	x1, x1, 2
	addi	x0, s0, -456
	add	x0, x0, x1
	lw	x0, 0(x0)
    la  x1, solution_count
    lw  x1, 0(x1)
	beq	x0, x1, .LBB4_4
	jal	.LBB4_3
.LBB4_3:
    jal .LBB4_3 # бесконечный цикл
.LBB4_4:                                #   in Loop: Header=BB4_1 Depth=1
	jal	.LBB4_5
.LBB4_5:                                #   in Loop: Header=BB4_1 Depth=1
	lw	x0, -424(s0)
	addi	x0, x0, 1
	sw	x0, -424(s0)
	jal	.LBB4_1
.LBB4_6:
	li	x0, 0
	ld	x1, 472(sp)
	ld	s0, 464(sp)
	addi	sp, sp, 480
	sret
	.size	main, .-main
	.type	finished,@object
	.section	.bss,"aw",@nobits
	.globl	finished
	.p2align	2

exit:
    # exit syscall
    ; li x7, 93
    sret
	.globl	backtrack
	.type	backtrack,@function
backtrack:
# %bb.0:
	addi	x1, x1, -448
	sd	x1, 440(sp)
	sd	s0, 432(sp)
	addi	s0, sp, 448
	sd	x0, -24(s0)
	sw	x1, -28(s0)
	sd	x2, -40(s0)
	ld	x0, -24(s0)
	lw	x1, -28(s0)
	ld	x2, -40(s0)
	; call	is_a_solution
    jal is_a_solution
	li	x1, 0
	beq	x0, x1, .LBB0_2
	jal .LBB0_1
.LBB0_1:
	ld	x0, -24(s0)
	lw	x1, -28(s0)
	; call	process_solution
    jal process_solution
	jal .LBB0_9
.LBB0_2:
	lw	x0, -28(s0)
	addi	x0, x0, 1
	sw	x0, -28(s0)
	ld	x0, -24(s0)
	lw	x1, -28(s0)
	ld	x2, -40(s0)
	addi	x3, s0, -440
	addi	x4, s0, -444
	; call	construct_candidates
    jal construct_candidates
	li	x0, 0
	sw	x0, -448(s0)
	jal .LBB0_3
.LBB0_3:                                # =>This Inner Loop Header: Depth=1
	lw	x0, -448(s0)
	lw	x1, -444(s0)
	bge	x0, x1, .LBB0_8
	jal .LBB0_4
.LBB0_4:                                #   in Loop: Header=BB0_3 Depth=1
	lw	x0, -448(s0)
	slli	x1, x0, 2
	addi	x0, s0, -440
	add	x0, x0, x1
	lw	x0, 0(x0)
	ld	x1, -24(s0)
	lw	x2, -28(s0)
	slli	x2, x2, 2
	add	x1, x1, x2
	sw	x0, 0(x1)
	ld	x0, -24(s0)
	lw	x1, -28(s0)
	ld	x2, -40(s0)
	; call	backtrack
    jal backtrack
.LBB0_10:                               #   in Loop: Header=BB0_3 Depth=1
	la	x0, finished
	lw	x0, 0(x0)
	li	x1, 0
	beq	x0, x1, .LBB0_6
	jal .LBB0_5
.LBB0_5:
	jal	.LBB0_9
.LBB0_6:                                #   in Loop: Header=BB0_3 Depth=1
	jal	.LBB0_7
.LBB0_7:                                #   in Loop: Header=BB0_3 Depth=1
	lw	x0, -448(s0)
	addi	x0, x0, 1
	sw	x0, -448(s0)
	jal	.LBB0_3
.LBB0_8:
	jal	.LBB0_9
.LBB0_9:
	ld	x1, 440(sp)
	ld	s0, 432(sp)
	addi	sp, sp, 448
	sret
	.size	backtrack, .-backtrack
	.globl	process_solution
	.type	process_solution,@function
process_solution:
# %bb.0:
	addi	sp, sp, -48
	sd	x1, 40(sp)
	sd	s0, 32(sp)
	addi	s0, sp, 48
	sd	x0, -32(s0)
	sw	x1, -36(s0)
.LBB1_1:
	la	x1, solution_count
	lw	x0, 0(x1)
	addi	x0, x0, 1
	sw	x0, 0(x1)
	lw	x0, -20(s0)
	ld	x1, 40(sp)
	ld	s0, 32(sp)
	addi	sp, sp, 48
	sret
	.size	process_solution, .-process_solution
	.globl	is_a_solution
	.type	is_a_solution,@function
is_a_solution:
# %bb.0:
	addi	sp, sp, -32
	sd	x1, 24(sp)
	sd	s0, 16(sp)
	addi	s0, sp, 32
	sd	x0, -24(s0)
	sw	x1, -28(s0)
	sw	x2, -32(s0)
	lw	x0, -28(s0)
	lw	x1, -32(s0)
	xor	x0, x0, x1
	; seqz	x0, x0
    slti a0, a0, 0 # Set a0 to 1 if a0 < 0, else 0
    xori a0, a0, -1 # if a0 == 0 then -1 else 0
    sltiu a0, a0, 1 # Set a0 to 1 if a0 < 1, else 0 (unsigned compare)
	ld	x1, 24(sp)
	ld	s0, 16(sp)
	addi sp, sp, 32
	sret
	.size	is_a_solution, .-is_a_solution
	.globl	construct_candidates
	.type	construct_candidates,@function
construct_candidates:
# %bb.0:
	addi	sp, sp, -80
	sd	x1, 72(sp)
	sd	s0, 64(sp)
	addi	s0, sp, 80
	sd	x0, -32(s0)
	sw	x1, -36(s0)
	sw	x2, -40(s0)
	sd	x3, -48(s0)
	sd	x4, -56(s0)
	ld	x1, -56(s0)
	li	x0, 0
	sw	x0, 0(x1)
	li	x0, 1
	sw	x0, -60(s0)
	jal	.LBB3_1
.LBB3_1:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB3_3 Depth 2
	lw	x1, -60(s0)
	lw	x0, -40(s0)
	blt	x0, x1, .LBB3_14
	jal	.LBB3_2
.LBB3_2:                                #   in Loop: Header=BB3_1 Depth=1
	li	x0, 1
	sw	x0, -68(s0)
	sw	x0, -64(s0)
	jal	.LBB3_3
.LBB3_3:                                #   Parent Loop BB3_1 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	lw	x0, -64(s0)
	lw	x1, -36(s0)
	bge	x0, x1, .LBB3_10
	jal	.LBB3_4
.LBB3_4:                                #   in Loop: Header=BB3_3 Depth=2
	lw	x0, -36(s0)
	lw	x1, -64(s0)
	sub	x0, x0, x1
    # abs
    slti x1, x0, 0
    bne x1, zero, .LBB3_abs_neg
    jal .LBB3_abs_end
    .LBB3_abs_neg:
        sub x0, zero, x0
    .LBB3_abs_end:
    # abs end
	sd	x0, -80(s0)
	lw	x0, -60(s0)
	ld	x1, -32(s0)
	lw	x2, -64(s0)
	slli	x2, x2, 2
	add	x1, x1, x2
	lw	x1, 0(x1)
	sub	x0, x0, x1
    # abs
    slti x1, x0, 0
    bne x1, zero, .LBB3_abs2_neg
    jal .LBB3_abs2_end
    .LBB3_abs2_neg:
        sub x0, zero, x0
    .LBB3_abs2_end:
    # abs end
	mv	x1, x0
	ld	x0, -80(s0)
	bne	x0, x1, .LBB3_6
	jal	.LBB3_5
.LBB3_5:                                #   in Loop: Header=BB3_3 Depth=2
	li	x0, 0
	sw	x0, -68(s0)
	jal	.LBB3_6
.LBB3_6:                                #   in Loop: Header=BB3_3 Depth=2
	lw	x0, -60(s0)
	ld	x1, -32(s0)
	lw	x2, -64(s0)
	slli	x2, x2, 2
	add	x1, x1, x2
	lw	x1, 0(x1)
	bne	x0, x1, .LBB3_8
	jal	.LBB3_7
.LBB3_7:                                #   in Loop: Header=BB3_3 Depth=2
	li	x0, 0
	sw	x0, -68(s0)
	jal	.LBB3_8
.LBB3_8:                                #   in Loop: Header=BB3_3 Depth=2
	jal	.LBB3_9
.LBB3_9:                                #   in Loop: Header=BB3_3 Depth=2
	lw	x0, -64(s0)
	addi	x0, x0, 1
	sw	x0, -64(s0)
	jal	.LBB3_3
.LBB3_10:                               #   in Loop: Header=BB3_1 Depth=1
	lw	x0, -68(s0)
	li	x1, 1
	bne	x0, x1, .LBB3_12
	jal	.LBB3_11
.LBB3_11:                               #   in Loop: Header=BB3_1 Depth=1
	lw	x0, -60(s0)
	ld	x1, -48(s0)
    ld  x2, -56(s0)
    lw  x2, 0(x2)
    slli x2, x2, 2
	add	x1, x1, x2
	sw	x0, 0(x1)
    ld  x1, -56(s0)
    lw  x0, 0(x1)
	addi	x0, x0, 1
	sw	x0, 0(x1)
	jal	.LBB3_12
.LBB3_12:                               #   in Loop: Header=BB3_1 Depth=1
	jal	.LBB3_13
.LBB3_13:                               #   in Loop: Header=BB3_1 Depth=1
	lw	x0, -60(s0)
	addi	x0, x0, 1
	sw	x0, -60(s0)
	jal	.LBB3_1
.LBB3_14:
	lw	x0, -20(s0)
	ld	x1, 72(sp)
	ld	s0, 64(sp)
	addi	sp, sp, 80
	sret
	.size	construct_candidates, .-construct_candidates
	.globl	main
	.type	main,@function
finished:
	.word	0
	.size	finished, 4
	.type	solution_count,@object
	.globl	solution_count
	.p2align	2
solution_count:
	.word	0
	.size	solution_count, 4
	.type	.L__const.main.compare,@object
	.section	.rodata
	.p2align	2
.L__const.main.compare:
	.word	1
	.word	0
	.word	0
	.word	2
	.word	10
	.word	4
	.word	40
    .word	92
	.size	.L__const.main.compare, 32
	
	.section .note.GNU-stack,"",@progbits
