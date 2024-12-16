.data

        # the numbers of a are the location of the queen in each row.
        # modify the first 0 to change the placement of the queen in the first row

a:      .word   0, 0, 0, 0, 0, 0, 0, 0
s:	.asciz	"Number of solutions="
error:	.asciz	"The position is not correct."

        # code
        .text
        .globl  main
main:   
        addi 	t0, x0, 8	# used for checking if first queen is out of range
        la      a0, a		# put address of solution array in a0
        li      a1, 1           # number of queens already placed
        
        lw 	t1, 0(a0)	# finding first value in a
        
        blt	t1, x0, out_of_range	# if first val of a is less than 0, out of range
        bge	t1, t0, out_of_range	# if first val of a is greater than or equal to 8, out of range
        
        jal	ra, solve_8queens	# solve
        
        add	s0, x0, a0	# load number of solutions into a0
        
        la	a0, a		# load address of a into a0
        jal 	ra, print_solution	# call print_solution (prints visual representation of board)
        
        add	t3, x0, x0	# counter
        addi	t4, x0, 8	# max of counter

# the loop below displays the solutions of queen placement for each row which are populated in a
loop:
	beq	t3, t4, display	# when loop is over, go to display
	slli	t2, t3, 2
	la	t1, a
	add	t2, t2, t1
	lw	a0, 0(t2)
	addi	a7, x0, 1
	ecall
	addi	a7, x0, 11
	addi	a0, x0, ' '
	ecall
	addi	t3, t3, 1
	beq	x0, x0, loop

display:	
	# print a newline
	addi	a7, x0, 11
	addi	a0, x0, '\n'
	ecall
	
	
	# print string
	addi	a7, x0, 4
	la	a0, s
	ecall
	
	# print number of solutions
	addi	a7, x0, 1
	add	a0, x0, s0
	ecall
	
	beq 	x0, x0, exit	# go to exit
	
out_of_range:
	addi	t0, x0, 8	# counter max
	addi	t1, x0, 0	# counter
error_loop:
	beq	t1, t0, end_error	# exit loop
	addi	a7, x0, 1	# printing integer
	add	a0, x0, x0	# loading 0 into a0
	ecall			# make call
	addi	a7, x0, 11	# printing ascii char
	addi	a0, x0, ' '	# space
	ecall			# make call
	addi	t1, t1, 1	# increment counter
	beq	x0, x0, error_loop	# go to top of loop

end_error:
	# print error
	addi	a7, x0, 11	# printing an ascii character
	addi	a0, x0, '\n'	# newline
	ecall			# make call
	
	addi	a7, x0, 4
	la	a0, error
	ecall

	# set a breakpoint here and check if any saved register was changed
        # exit
exit:   addi    a7, x0, 10      
        ecall

#######################

# function to print visual representation of board
print_solution:
	addi	sp, sp, -8	# allocate space on stack for 2 words
	sw	s0, 4(sp)	# save s0 on the stack
	sw	ra, 0(sp)	# save ra on the stack
	
	add	s0, x0, a0	# save &a in s0
	add	t0, x0, x0	# counter
	addi	t1, x0, 8	# max counter
	
p_outer:
	beq	t0, t1, print_exit
	slli	t2, t0, 2	# multiply counter * 4
	add	t2, s0, t2	# get addr from a (&a + i * 4)
	lw	t3, 0(t2)	# load from a
	
	add	t4, x0, x0	# inner loop counter
p_inner:
	beq	t4, t1, end_outer	# end the inner loop
	bne	t4, t3, blank
	addi	a7, x0, 11	# printing ascii char
	addi	a0, x0, '*'	# using newline
	ecall			# call to print
	beq	x0, x0, end_inner	# go to end_inner
blank:
	addi	a7, x0, 11	# printing ascii char
	addi	a0, x0, '-'	# using newline
	ecall			# call to print
end_inner:
	addi	a7, x0, 11	# printing ascii char
	addi	a0, x0, ' '	# space
	# ecall
	addi	t4, t4, 1	# increment inner loop counter
	beq	x0, x0, p_inner	# go to top of inner loop
	
	
end_outer:
	addi	a7, x0, 11	# printing ascii char
	addi	a0, x0, '\n'	# using newline
	ecall			# call to print
	addi	t0, t0, 1	# increment counter
	beq	x0, x0, p_outer	# go back to top of loop
	
print_exit:
	lw	s0, 4(sp)	# load s0 from the stack
	lw	ra, 0(sp)	# load ra from the stack
	addi	sp, sp, 8	# deallocate space on stack
	jr 	ra
	
#######################

# solver function
solve_8queens:
	addi	sp, sp, -20	# allocate space for 5 words on the stack
	sw	s0, 16(sp)	# preserve s0 on the stack
	sw	s1, 12(sp)	# preserve s1 on the stack
	sw	s2, 8(sp)	# preserve s2 on the stack
	sw	s3, 4(sp)	# preserve s3 on the stack
	sw	ra, 0(sp)	# preserve ra on the stack
	
	add	s0, x0, a0	# save address of a in s0
	add	s1, x0, a1	# save k in s1
	add	s2, x0, x0	# save solution counter in s2
	add	s3, x0, x0	# save j in s3 (loop counter)
	
	addi	t6, x0, 8	# max value of k is 8
	
	blt	s1, t6, q_loop	# if k hits eight, we are done, otherwise loop
	addi	s2, s2, 1	# increment counter
	add	a0, s2, x0	# set a0 to counter
	beq	x0, x0, q_exit	# exit and return
	
q_loop:	
	blt	s3, t6, q_cont	# if j < 8, continue to normal loop
	add	a0, s2, x0	# set a0 to the solution counter
	beq	x0, x0, q_exit	# exit the function
	
q_cont:
	add	a0, s0, x0	# set a0 to &a
	add	a1, s1, x0	# set a1 to k
	add	a2, s3, x0	# set a2 to j
	jal	ra, is_valid	# call is_valid
	addi	t3, x0, 1	# set t3 to 1
	blt	a0, t3, q_el	# if is_valid returns 0, go to end of this iteration of the loop
	
	slli	t2, s1, 2	# k * 4
	add	t2, t2, s0	# a[k]
	sw	s3, 0(t2)	# a[k] = j
	
	add	a0, s0, x0	# save a in a0
	addi	a1, s1, 1	# save k + 1 in a1
	jal	ra, solve_8queens	# recursively call solve_8queens
	# lw	s2, 8(sp)
	add	s2, s2, a0	# add counter + result from recursive call
	beq	s2, x0, q_el	# continue to end of loop if counter is still 0
	add	a0, s2, x0	# set a0 to counter
	beq	x0, x0, q_exit	# exit function
	
q_el:
	addi	s3, s3, 1	# increment j
	beq	x0, x0, q_loop	# go back to top of loop
	
q_exit:        
	lw	s0, 16(sp)	# load s0 from stack
	lw	s1, 12(sp)	# load s1 from stack
	lw	s2, 8(sp)	# load s2 from stack
	lw	s3, 4(sp)	# load s3 from stack
	lw	ra, 0(sp)	# preserve ra from stack
	addi	sp, sp, 20	# deallocate space on stack
	jr	ra		# return from function
        
#######################

# calculates the absolute value of the difference between two numbers
difference:
	addi 	sp, sp, -4 	# allocate space for 1 word
	sw 	ra, 0(sp)	# preserve ra on stack
	bge 	a1, a0, d_else	# if i >= j, go to else
	sub 	a0, a0, a1	# j - i
	beq	x0, x0, d_exit	# go to exit
d_else:	sub	a0, a1, a0	# i - j
d_exit:	
	lw	ra, 0(sp)	# load ra from stack
	addi	sp, sp, 4	# deallocate space from stack
	jr 	ra		# return from function
	
#######################

# checks if a placement of a queen is valid based on the placement of queens in rows above it
is_valid:
	addi 	sp, sp, -24	# allocate space for 6 words on stack
	sw	s0, 20(sp)	# preserve s0 on stack
	sw	s1, 16(sp)	# preserve s1 on stack
	sw	s2, 12(sp)	# preserve s2 on stack
	sw	s3, 8(sp)	# preserve s3 on stack
	sw	s4, 4(sp)	# preserve s4 on stack
	sw	ra, 0(sp)	# preserve ra on stack
	
	add	s0, x0, a0	# save address of a in s0
	add	s1, x0, a1	# save row in s1
	add	s2, x0, a2	# save column in s2
	add	s3, x0, x0	# save i in s3 (starts at 0)
	
iv_loop:
	blt	s3, s1, iv_cont	# continue to loop if i < row
	addi	a0, x0, 1	# set return value to true
	beq	x0, x0, iv_exit	# exit function
iv_cont:
	slli	a0, s3, 2	# i * 4
	add	a0, s0, a0	# add &a + i*4
	lw	s4, 0(a0)	# load from a0 into s4 to get a[i]
	bne 	s4, s2, iv_t2	# go to second conditional test
	beq	x0, x0, iv_f	# break out of loop
iv_t2:
	add	a0, s4, x0	# a0 set to a[i]
	add	a1, x0, s2	# a1 set to column
	jal	ra, difference	# call difference
	add	t1, a0, x0	# save return in t1 --> consider changing to saved register
	
	add	a0, s3, x0	# first arg is i
	add	a1, s1, x0	# second arg is row
	jal	ra, difference	# call difference
	beq	a0, t1, iv_f	# need to break and return false
	beq	x0, x0, iv_el	# go to end loop

iv_f:
	
	addi	a0, x0, 0	# set return value to 0
	beq	x0, x0, iv_exit	# exit function
	
iv_el:	
	addi	s3, s3, 1	# increment counter
	beq	x0, x0, iv_loop	# go to top of loop

iv_exit:	
	lw	s0, 20(sp)	# load s0 from stack
	lw	s1, 16(sp)	# load s1 from stack
	lw	s2, 12(sp)	# load s2 from stack
	lw	s3, 8(sp)	# load s3 from stack
	lw	s4, 4(sp)	# load s4 from stack
	lw	ra, 0(sp)	# load ra from stack
	addi	sp, sp, 24	# deallocate space on stack
	jr 	ra		# return from function