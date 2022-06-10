# DO NOT REMOVE THIS LINE WHEN SUBMITTING
.import input.S

.data

.globl ans
ans:
    .word 0

.text  # code starts here

.globl main
main:
    # Load data from input.S:
    la a2 str1
    la a3 str2
    lw a4 len1 # a4=len1
    lw a5 len2 # a5=len2
    
    lw s0 ans # s0=ans=0
    jal ra func

print:
    # Print answer and exit
    li a0 1
    mv a1 s0
    ecall
    li a0 10
    ecall

max:
    bge a0 a1 Re
    mv a0 a1
Re:
    ret

func:
    li t0 0 # t0=i
    j L1

L1: # Outer loop
    bge t0 a4 L1_exit # If i>=len1 then exit
    li t1 0 # t1=j
    j L2

L2: # Inner loop
    bge t1 a5 L2_exit # If j>=len2 then exit
    li t2 0 # t2=k=0
    j while

while:
    add t4 t0 t2 # t4=i+k
    bge t4 a4 while_exit # If i+k>=len1, then exit while loop
    slli s1 t4 2 # Get the offset of i+k, store in s1
    add t5 s1 a2 # Get the address of str1[i+k]
    lw t5 0(t5) # t5=str1[i+k]

    add t6 t1 t2 # t6=j+k
    bge t6 a5 while_exit # If j+k>=len2, then exit while loop
    slli s2 t6 2 # Get the offset of j+k, store in s2
    add a6 s2 a3 # Get the address of str2[j+k]
    lw a6 0(a6) # a6=str2[j+k]

    bne t5 a6 while_exit # If str1[i+k]!=str2[j+k], then exit while loop

    addi t2 t2 1 # k++
    j while

while_exit:
    mv a0 t2 # a0=k
    mv a1 s0 # a1=ans
    jal ra max
    mv s0 a0 # ans=max(k,ans)
    addi t1 t1 1 # j++
    j L2

L2_exit:
    addi t0 t0 1 # i++
    j L1

L1_exit:
    j print
