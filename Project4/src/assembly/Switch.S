.data
play: .string "Play"

.section .text
.globl Switch
.type Switch, @function
Switch:
    addi sp, sp, -52
    nop
    sw ra, 0(sp)    # store ra
    sw s0, 4(sp)    # store s0
    nop
    sw s1, 8(sp)    # store s1
    nop
    sw s2, 12(sp)   # store s2
    nop
    sw s3, 16(sp)   # store s3
    sw s4, 20(sp)   # store s4
    nop
    sw s5, 24(sp)   # store s5
    nop
    sw s6, 28(sp)   # store s6
    sw s7, 32(sp)   # store s7
    nop
    sw s8, 36(sp)   # store s8
    nop
    sw s9, 40(sp)   # store s9
    sw s10, 44(sp)  # store s10
    nop
    sw s11, 48(sp)  # store s11
    nop

    # Fill black
    li a0, 38; # a0=0
    nop
    li a1, 43; # a1=0
    li a2, 72; # a2=88
    nop
    li a3, 65; # a3=20
    li a4, 0x0000; # a4=black
    jal LCD_Fill
    nop

    # Print Play
    nop
    li a0, 40; # a0=20
    li a1, 45;  # a1=5
    nop
    la a2, play; # a2=address of Play
    li a3, 0x265E # a3=color
    nop
    jal LCD_ShowString
    nop

    # Rectangle
    li a0, 88; # a0=0
    nop
    li a1, 43; # a1=0
    li a2, 122; # a2=88
    nop
    li a3, 65; # a3=20
    li a4, 0xE0B7; # a4=color
    jal LCD_DrawRectangle

    # Delay
    li a0, 150; # a0=150
    jal delay_1ms

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
    nop