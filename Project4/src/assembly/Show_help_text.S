.data
help: .string "Help"
sent: .string "Press button0 to jump and button1 to prone. Press button0to exit help."

.section .text
.globl Show_help_text
.type Show_help_text, @function
Show_help_text:
    addi sp, sp, -52
    nop
    sw ra, 0(sp)    # store ra
    sw s0, 4(sp)    # store s0
    nop
    sw s1, 8(sp)    # store s1
    sw s2, 12(sp)   # store s2
    nop
    sw s3, 16(sp)   # store s3
    sw s4, 20(sp)   # store s4
    nop
    sw s5, 24(sp)   # store s5
    sw s6, 28(sp)   # store s6
    nop
    sw s7, 32(sp)   # store s7
    sw s8, 36(sp)   # store s8
    nop
    sw s9, 40(sp)   # store s9
    sw s10, 44(sp)  # store s10
    nop
    sw s11, 48(sp)  # store s11

    li a0, 0x0000; # a0 = black
    jal LCD_Clear

    # Print help
    nop
    li a0, 65; # a0=65
    li a1, 0;  # a1=0
    nop
    la a2, help; # a2=address of help
    li a3, 0x265E # a3 = color
    nop
    jal LCD_ShowString
    nop

    # Print sentence
    nop
    li a0, 0; # a0=0
    li a1, 15;  # a1=15
    nop
    la a2, sent; # a2=address of sentence
    li a3, 0x265E # a3 = color
    nop
    jal LCD_ShowString
    nop

    lw ra, 0(sp)    # load ra
    nop
    lw s0, 4(sp)    # load s0
    lw s1, 8(sp)    # load s1
    nop
    lw s2, 12(sp)   # load s2
    lw s3, 16(sp)   # load s3
    nop
    lw s4, 20(sp)   # load s4
    lw s5, 24(sp)   # load s5
    nop
    lw s6, 28(sp)   # load s6
    lw s7, 32(sp)   # load s7
    nop
    lw s8, 36(sp)   # load s8
    lw s9, 40(sp)   # load s9
    nop
    lw s10, 44(sp)  # load s10
    lw s11, 48(sp)  # load s11
    nop
    addi sp, sp, 52
    ret