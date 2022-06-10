.data
shouldben1:	.asciiz "Should be -1, and firstposshift and firstposmask returned: "
shouldbe0:	.asciiz "Should be 0 , and firstposshift and firstposmask returned: "
shouldbe16:	.asciiz "Should be 16, and firstposshift and firstposmask returned: "
shouldbe31:	.asciiz "Should be 31, and firstposshift and firstposmask returned: "

.text
main:
	la	a1, shouldbe31
	jal	print_str
	lui	a1, 0x80000	# should be 31
	jal	first1posshift
	mv	a1, a0
	jal	print_int
	jal	print_space

	lui	a1, 0x80000
	jal	first1posmask
	mv	a1, a0
	jal	print_int
	jal	print_newline

	la	a1, shouldbe16
	jal	print_str
	lui	a1, 0x0010	# should be 16
	jal	first1posshift
	mv	a1, a0
	jal	print_int
	jal	print_space

	lui	a1, 0x0010
	jal	first1posmask
	mv	a1, a0
	jal	print_int
	jal	print_newline

	la	a1, shouldbe0
	jal	print_str
	li	a1, 1		# should be 0
	jal	first1posshift
	mv	a1, a0
	jal	print_int
	jal	print_space

	li	a1, 1
	jal	first1posmask
	mv	a1, a0
	jal	print_int
	jal	print_newline

	la	a1, shouldben1
	jal	print_str
	mv	a1, zero		# should be -1
	jal	first1posshift
	mv	a1, a0
	jal	print_int
	jal	print_space

	mv	a1, zero
	jal	first1posmask
	mv	a1, a0
	jal	print_int
	jal	print_newline

	li	a0, 10
	ecall

first1posshift:
	li t0 1 # t0=0x00000001
    li t1 0 # t1 is the pos of 1
    li t3 32 # t3 indicates no left most 1
    j while1

while1:
    and t2 t0 a1 # t2 indicates whether the left most 1 is found
    bne t2 zero get_ans
    addi t1 t1 1
    slli t0 t0 1
    beq t1 t3 ne1
    j while1

get_ans:
    mv a0 t1
    jalr ra

ne1:
    li a0 -1
    jalr ra

first1posmask:
	li t0 0x80000000 # t0=0x80000000
    li t1 31 # t1 is the pos of 1
    li t3 -1 # t3 indicates no left most 1
    j while2

while2:
    and t2 t0 a1 # t2 indicates whether the left most 1 is found
    bne t2 zero get_ans
    addi t1 t1 -1
    srli t0 t0 1
    beq t1 t3 ne1
    j while2

print_int:
	mv	a1, a0
	li	a0, 1
	ecall
	jr	ra

print_str:
	li	a0, 4
	ecall
	jr	ra

print_space:
	li	a1, ' '
	li	a0, 11
	ecall
	jr	ra

print_newline:
	li	a1, '\n'
	li	a0, 11
	ecall
	jr	ra
