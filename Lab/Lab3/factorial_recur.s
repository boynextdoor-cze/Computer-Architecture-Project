.data
test_input: .word 3 6 7 8

.text
main:
	add t0, x0, x0
	addi t1, x0, 4
	la t2, test_input
main_loop:
	beq t0, t1, main_exit
	slli t3, t0, 2
	add t4, t2, t3
	lw a0, 0(t4) # a0=n

	addi sp, sp, -20
	sw t0, 0(sp)
	sw t1, 4(sp)
	sw t2, 8(sp)
	sw t3, 12(sp)
	sw t4, 16(sp)

	jal ra, factorial

	lw t0, 0(sp)
	lw t1, 4(sp)
	lw t2, 8(sp)
	lw t3, 12(sp)
	lw t4, 16(sp)
	addi sp, sp, 20

	addi a1, a0, 0
	addi a0, x0, 1
	ecall # Print Result
	addi a1, x0, ' '
	addi a0, x0, 11
	ecall
	
	addi t0, t0, 1
	jal x0, main_loop
main_exit:  
	addi a0, x0, 10
	ecall # Exit

factorial:
    addi sp, sp, -8
    sw a0, 4(sp)
    sw x1, 0(sp)
    addi t0, a0, -1
    bne t0, x0, Recur
    addi a0, x0, 1
    addi sp, sp, 8
    jalr x0, x1, 0
Recur:
    # Recur into next layer
	addi a0, a0, -1
    jal x1, factorial
    # traceback:
    mv t1, a0 # t1 = n-1
    lw x1, 0(sp)
    lw a0, 4(sp)
    addi sp, sp, 8
    mul a0, a0, t1
    jalr x0, x1, 0