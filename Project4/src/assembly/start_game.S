.section .text
.globl start_game
.type start_game, @function
start_game:
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
    
    li a0,0x0000;
    nop
    jal LCD_Clear       # clear the screen
    jal display_mode    # go to the display mode
    nop
    mv s0,a0;           # s1=mode
    li a0,0x0000;
    nop
    jal LCD_Clear       # clear the screen
    li a0,100;          # delay 100ms
    nop
    jal delay_1ms       # go to the delay
    li a0,0;            # x=0
    nop
    li a1,70;           # y=70
    li a2,159;          # x'=159
    nop
    li a3,70;           # y'=70
    li a4,0xFFFF;       # color=white
    nop
    jal LCD_DrawLine    # go to drawing
    beq s0,zero,next;   # if mode=0,go to next
    nop
    li a0, 8;           # a0=8
    li a1, 15;          # a1=15
    nop
    li a2,15;           # a2=15
    li a3,100;          # a3=100
    nop
    li a4,10;           # a4=10
    jal play            # go to play
    nop
    j Exit              # go to exit
next:
    nop
    li a0, 2;           # a0=2
    li a1, 15;          # a1=15
    nop
    li a2,30;           # a2=30
    li a3,200;          # a3=200
    nop
    li a4,20;           # a4=20s
    jal play            # go to play
    nop
Exit:
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