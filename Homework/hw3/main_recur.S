# DO NOT REMOVE THIS LINE WHEN SUBMITTING
.import input.S

.data

.globl ans
ans:
    .word 0

.text  # code starts here

.globl main
main:
    la a2 str1
    la a3 str2
    lw a4 len1 # a4=len1
    lw a5 len2 # a5=len2
    lw s2 ans # s2=ans
    li a7 0 # a7=count, the current length of LCS
    mv a0 a4 # a0=i
    mv a1 a5 # a1=j
    jal ra DP

    # Print answer and exit
    li a0 1
    mv a1 s2
    ecall
    li a0 10
    ecall

DP:
    # Store ra, i, j, count in stack
    addi sp sp -16
    sw ra 12(sp)
    sw s0 8(sp)
    sw s1 4(sp)
    sw a7 0(sp)
    mv s0 a0 # s0=temp_i
    mv s1 a1 # s1=temp_j
    # Check if i or j is out of range:
    # If so, directly return
    blt a0 x0 Traceback
    blt a1 x0 Traceback
    # Retrieve s1[i]:
    slli t0 a0 2 # t0=i*4
    add t1 t0 a2 # t1=address of s1[i]
    lw t2 0(t1) # t2=s1[i]
    # Retrieve s2[j]:
    slli t3 a1 2 # t3=j*4
    add t4 t3 a3 # t4=address of s2[j]
    lw t5 0(t4) # t5=s2[j]
    bne t2 t5 Branch2 # If s1[i]!=s2[j], recur for DP(i-1,j,0) and DP(i,j-1,0)
    # If s1[i]=s2[j], update ans and recur for DP(i-1,j-1,count+1)
    addi t6 a7 1 # t6=count+1
    ble t6 s2 Branch1 # If count+1<=ans, then directly goto Branch 1
    mv s2 t6 # Update ans, ans=count+1
    j Branch1

Branch1: # Recursion for DP(i-1,j-1,count+1)
    addi a0 s0 -1 # i-1
    addi a1 s1 -1 # j-1
    addi a7 a7 1 # count+1
    jal ra DP

Branch2: # Recursion for DP(i-1,j,0) and DP(i,j-1,0)
    # DP(i-1,j,0):
    addi a0 s0 -1 # i-1
    mv a1 s1 # j
    li a7 0 # count=0
    jal ra DP
    # DP(i,j-1,0):
    mv a0 s0 # i
    addi a1 s1 -1 # j-1
    li a7 0 # count=0
    jal ra DP
    j Traceback

Traceback:
    lw a7 0(sp)
    lw s1 4(sp)
    lw s0 8(sp)
    lw ra 12(sp)
    addi sp sp 16
    jalr x0 ra 0