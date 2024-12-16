	.text
	.attribute	4, 16
	.attribute	5, "rv64i2p0_m2p0"
	.file	"8-queens.c"
	.globl	backtrack                       # -- Begin function backtrack
	.p2align	2
	.type	backtrack,@function
backtrack:                              # @backtrack
# %bb.0:
	addi	sp, sp, -448
	sd	ra, 440(sp)                     # 8-byte Folded Spill
	sd	s0, 432(sp)                     # 8-byte Folded Spill
	addi	s0, sp, 448
                                        # kill: def $x13 killed $x11
	sd	a0, -24(s0)
	sw	a1, -28(s0)
	sd	a2, -40(s0)
	ld	a0, -24(s0)
	lw	a1, -28(s0)
	ld	a2, -40(s0)
	call	is_a_solution
	li	a1, 0
	beq	a0, a1, .LBB0_2
	j	.LBB0_1
.LBB0_1:
	ld	a0, -24(s0)
	lw	a1, -28(s0)
	call	process_solution
	j	.LBB0_9
.LBB0_2:
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -28(s0)
	ld	a0, -24(s0)
	lw	a1, -28(s0)
	ld	a2, -40(s0)
	addi	a3, s0, -440
	addi	a4, s0, -444
	call	construct_candidates
	li	a0, 0
	sw	a0, -448(s0)
	j	.LBB0_3
.LBB0_3:                                # =>This Inner Loop Header: Depth=1
	lw	a0, -448(s0)
	lw	a1, -444(s0)
	bge	a0, a1, .LBB0_8
	j	.LBB0_4
.LBB0_4:                                #   in Loop: Header=BB0_3 Depth=1
	lw	a0, -448(s0)
	slli	a1, a0, 2
	addi	a0, s0, -440
	add	a0, a0, a1
	lw	a0, 0(a0)
	ld	a1, -24(s0)
	lw	a2, -28(s0)
	slli	a2, a2, 2
	add	a1, a1, a2
	sw	a0, 0(a1)
	ld	a0, -24(s0)
	lw	a1, -28(s0)
	ld	a2, -40(s0)
	call	backtrack
.LBB0_10:                               #   in Loop: Header=BB0_3 Depth=1
                                        # Label of block must be emitted
	auipc	a0, %pcrel_hi(finished)
	addi	a0, a0, %pcrel_lo(.LBB0_10)
	lw	a0, 0(a0)
	li	a1, 0
	beq	a0, a1, .LBB0_6
	j	.LBB0_5
.LBB0_5:
	j	.LBB0_9
.LBB0_6:                                #   in Loop: Header=BB0_3 Depth=1
	j	.LBB0_7
.LBB0_7:                                #   in Loop: Header=BB0_3 Depth=1
	lw	a0, -448(s0)
	addiw	a0, a0, 1
	sw	a0, -448(s0)
	j	.LBB0_3
.LBB0_8:
	j	.LBB0_9
.LBB0_9:
	ld	ra, 440(sp)                     # 8-byte Folded Reload
	ld	s0, 432(sp)                     # 8-byte Folded Reload
	addi	sp, sp, 448
	ret
.Lfunc_end0:
	.size	backtrack, .Lfunc_end0-backtrack
                                        # -- End function
	.globl	process_solution                # -- Begin function process_solution
	.p2align	2
	.type	process_solution,@function
process_solution:                       # @process_solution
# %bb.0:
	addi	sp, sp, -48
	sd	ra, 40(sp)                      # 8-byte Folded Spill
	sd	s0, 32(sp)                      # 8-byte Folded Spill
	addi	s0, sp, 48
                                        # kill: def $x12 killed $x11
	sd	a0, -32(s0)
	sw	a1, -36(s0)
.LBB1_1:                                # Label of block must be emitted
	auipc	a1, %pcrel_hi(solution_count)
	addi	a1, a1, %pcrel_lo(.LBB1_1)
	lw	a0, 0(a1)
	addiw	a0, a0, 1
	sw	a0, 0(a1)
	lw	a0, -20(s0)
	ld	ra, 40(sp)                      # 8-byte Folded Reload
	ld	s0, 32(sp)                      # 8-byte Folded Reload
	addi	sp, sp, 48
	ret
.Lfunc_end1:
	.size	process_solution, .Lfunc_end1-process_solution
                                        # -- End function
	.globl	is_a_solution                   # -- Begin function is_a_solution
	.p2align	2
	.type	is_a_solution,@function
is_a_solution:                          # @is_a_solution
# %bb.0:
	addi	sp, sp, -32
	sd	ra, 24(sp)                      # 8-byte Folded Spill
	sd	s0, 16(sp)                      # 8-byte Folded Spill
	addi	s0, sp, 32
                                        # kill: def $x13 killed $x12
                                        # kill: def $x13 killed $x11
	sd	a0, -24(s0)
	sw	a1, -28(s0)
	sw	a2, -32(s0)
	lw	a0, -28(s0)
	lw	a1, -32(s0)
	xor	a0, a0, a1
	seqz	a0, a0
	ld	ra, 24(sp)                      # 8-byte Folded Reload
	ld	s0, 16(sp)                      # 8-byte Folded Reload
	addi	sp, sp, 32
	ret
.Lfunc_end2:
	.size	is_a_solution, .Lfunc_end2-is_a_solution
                                        # -- End function
	.globl	construct_candidates            # -- Begin function construct_candidates
	.p2align	2
	.type	construct_candidates,@function
construct_candidates:                   # @construct_candidates
# %bb.0:
	addi	sp, sp, -80
	sd	ra, 72(sp)                      # 8-byte Folded Spill
	sd	s0, 64(sp)                      # 8-byte Folded Spill
	addi	s0, sp, 80
                                        # kill: def $x15 killed $x12
                                        # kill: def $x15 killed $x11
	sd	a0, -32(s0)
	sw	a1, -36(s0)
	sw	a2, -40(s0)
	sd	a3, -48(s0)
	sd	a4, -56(s0)
	ld	a1, -56(s0)
	li	a0, 0
	sw	a0, 0(a1)
	li	a0, 1
	sw	a0, -60(s0)
	j	.LBB3_1
.LBB3_1:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB3_3 Depth 2
	lw	a1, -60(s0)
	lw	a0, -40(s0)
	blt	a0, a1, .LBB3_14
	j	.LBB3_2
.LBB3_2:                                #   in Loop: Header=BB3_1 Depth=1
	li	a0, 1
	sw	a0, -68(s0)
	sw	a0, -64(s0)
	j	.LBB3_3
.LBB3_3:                                #   Parent Loop BB3_1 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	lw	a0, -64(s0)
	lw	a1, -36(s0)
	bge	a0, a1, .LBB3_10
	j	.LBB3_4
.LBB3_4:                                #   in Loop: Header=BB3_3 Depth=2
	lw	a0, -36(s0)
	lw	a1, -64(s0)
	subw	a0, a0, a1
	call	abs@plt
	sd	a0, -80(s0)                     # 8-byte Folded Spill
	lw	a0, -60(s0)
	ld	a1, -32(s0)
	lw	a2, -64(s0)
	slli	a2, a2, 2
	add	a1, a1, a2
	lw	a1, 0(a1)
	subw	a0, a0, a1
	call	abs@plt
	mv	a1, a0
	ld	a0, -80(s0)                     # 8-byte Folded Reload
	bne	a0, a1, .LBB3_6
	j	.LBB3_5
.LBB3_5:                                #   in Loop: Header=BB3_3 Depth=2
	li	a0, 0
	sw	a0, -68(s0)
	j	.LBB3_6
.LBB3_6:                                #   in Loop: Header=BB3_3 Depth=2
	lw	a0, -60(s0)
	ld	a1, -32(s0)
	lw	a2, -64(s0)
	slli	a2, a2, 2
	add	a1, a1, a2
	lw	a1, 0(a1)
	bne	a0, a1, .LBB3_8
	j	.LBB3_7
.LBB3_7:                                #   in Loop: Header=BB3_3 Depth=2
	li	a0, 0
	sw	a0, -68(s0)
	j	.LBB3_8
.LBB3_8:                                #   in Loop: Header=BB3_3 Depth=2
	j	.LBB3_9
.LBB3_9:                                #   in Loop: Header=BB3_3 Depth=2
	lw	a0, -64(s0)
	addiw	a0, a0, 1
	sw	a0, -64(s0)
	j	.LBB3_3
.LBB3_10:                               #   in Loop: Header=BB3_1 Depth=1
	lw	a0, -68(s0)
	li	a1, 1
	bne	a0, a1, .LBB3_12
	j	.LBB3_11
.LBB3_11:                               #   in Loop: Header=BB3_1 Depth=1
	lw	a0, -60(s0)
	ld	a1, -48(s0)
	ld	a2, -56(s0)
	lw	a2, 0(a2)
	slli	a2, a2, 2
	add	a1, a1, a2
	sw	a0, 0(a1)
	ld	a1, -56(s0)
	lw	a0, 0(a1)
	addiw	a0, a0, 1
	sw	a0, 0(a1)
	j	.LBB3_12
.LBB3_12:                               #   in Loop: Header=BB3_1 Depth=1
	j	.LBB3_13
.LBB3_13:                               #   in Loop: Header=BB3_1 Depth=1
	lw	a0, -60(s0)
	addiw	a0, a0, 1
	sw	a0, -60(s0)
	j	.LBB3_1
.LBB3_14:
	lw	a0, -20(s0)
	ld	ra, 72(sp)                      # 8-byte Folded Reload
	ld	s0, 64(sp)                      # 8-byte Folded Reload
	addi	sp, sp, 80
	ret
.Lfunc_end3:
	.size	construct_candidates, .Lfunc_end3-construct_candidates
                                        # -- End function
	.globl	main                            # -- Begin function main
	.p2align	2
	.type	main,@function
main:                                   # @main
# %bb.0:
	addi	sp, sp, -480
	sd	ra, 472(sp)                     # 8-byte Folded Spill
	sd	s0, 464(sp)                     # 8-byte Folded Spill
	addi	s0, sp, 480
	li	a0, 0
	sw	a0, -20(s0)
.LBB4_7:                                # Label of block must be emitted
	auipc	a0, %pcrel_hi(.L__const.main.compare)
	addi	a0, a0, %pcrel_lo(.LBB4_7)
	sd	a0, -464(s0)                    # 8-byte Folded Spill
	lwu	a2, 24(a0)
	lwu	a1, 28(a0)
	slli	a1, a1, 32
	or	a1, a1, a2
	sd	a1, -432(s0)
	lwu	a2, 16(a0)
	lwu	a1, 20(a0)
	slli	a1, a1, 32
	or	a1, a1, a2
	sd	a1, -440(s0)
	lwu	a2, 8(a0)
	lwu	a1, 12(a0)
	slli	a1, a1, 32
	or	a1, a1, a2
	sd	a1, -448(s0)
	lwu	a1, 0(a0)
	lwu	a0, 4(a0)
	slli	a0, a0, 32
	or	a0, a0, a1
	sd	a0, -456(s0)
	li	a0, 1
	sw	a0, -424(s0)
	j	.LBB4_1
.LBB4_1:                                # =>This Inner Loop Header: Depth=1
	lw	a1, -424(s0)
	li	a0, 8
	blt	a0, a1, .LBB4_6
	j	.LBB4_2
.LBB4_2:                                #   in Loop: Header=BB4_1 Depth=1
.LBB4_8:                                #   in Loop: Header=BB4_1 Depth=1
                                        # Label of block must be emitted
	auipc	a0, %pcrel_hi(solution_count)
	addi	a0, a0, %pcrel_lo(.LBB4_8)
	sd	a0, -472(s0)                    # 8-byte Folded Spill
	li	a1, 0
	sw	a1, 0(a0)
	lw	a2, -424(s0)
	addi	a0, s0, -420
	call	backtrack
	ld	a1, -472(s0)                    # 8-byte Folded Reload
	lw	a0, -424(s0)
	addiw	a0, a0, -1
	slli	a2, a0, 2
	addi	a0, s0, -456
	add	a0, a0, a2
	lw	a0, 0(a0)
	lw	a1, 0(a1)
	beq	a0, a1, .LBB4_4
	j	.LBB4_3
.LBB4_3:
	call	abort@plt
.LBB4_4:                                #   in Loop: Header=BB4_1 Depth=1
	j	.LBB4_5
.LBB4_5:                                #   in Loop: Header=BB4_1 Depth=1
	lw	a0, -424(s0)
	addiw	a0, a0, 1
	sw	a0, -424(s0)
	j	.LBB4_1
.LBB4_6:
	li	a0, 0
	ld	ra, 472(sp)                     # 8-byte Folded Reload
	ld	s0, 464(sp)                     # 8-byte Folded Reload
	addi	sp, sp, 480
	ret
.Lfunc_end4:
	.size	main, .Lfunc_end4-main
                                        # -- End function
	.type	finished,@object                # @finished
	.section	.sbss,"aw",@nobits
	.globl	finished
	.p2align	2
finished:
	.word	0                               # 0x0
	.size	finished, 4

	.type	solution_count,@object          # @solution_count
	.globl	solution_count
	.p2align	2
solution_count:
	.word	0                               # 0x0
	.size	solution_count, 4

	.type	.L__const.main.compare,@object  # @__const.main.compare
	.section	.rodata.cst32,"aM",@progbits,32
	.p2align	2
.L__const.main.compare:
	.word	1                               # 0x1
	.word	0                               # 0x0
	.word	0                               # 0x0
	.word	2                               # 0x2
	.word	10                              # 0xa
	.word	4                               # 0x4
	.word	40                              # 0x28
	.word	92                              # 0x5c
	.size	.L__const.main.compare, 32

	.ident	"Ubuntu clang version 14.0.0-1ubuntu1.1"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym backtrack
	.addrsig_sym process_solution
	.addrsig_sym is_a_solution
	.addrsig_sym construct_candidates
	.addrsig_sym abs
	.addrsig_sym abort
	.addrsig_sym finished
	.addrsig_sym solution_count
