#   Project 1.2 RVC instructions to RISC-V instructions in RISC-V

#   You can suppose you will get the number of machine codes you should translate from 
#   "lines_of_rvc_codes" and get the 16 or 32 bits-machine codes from "rvc_codes". 
#   Both of them are stored in file "input.S"

#   When we test your project, we will change the input files content.

# DO NOT REMOVE THIS LINE WHEN SUBMITTING
.import input.S

# Your code to convert the RVC codes to RVI codes.
Memory_allocation:
    la      a1 lines_of_rvc_codes
    lw      a1 0(a1) # a1 = lines_of_rvc_codes
    li      a0 0x3cc
    li      a2 4
    li      a6 2
    ecall # malloc "lines*4" bytes to store instructions
    # Now the pointer of array, called instr[], is a0
    mv      s0 a0 # Let s0 be the starting pointer of instr[], always fixed
    li      a0 0x3cc
    ecall # malloc "lines*4" bytes to store the size of bytes of each instruction
    # Now the pointer of array, called size[], is a0
    mv      s2 a0 # Let s2 be the starting pointer of size[], always fixed

Initialization:
    mv      s11 a1 # s11 = lines_of_rvc_codes
    li      a2 0 # a2 indicates the starting position of reading instructions
    la      s1 rvc_codes # s1 = the address of rvc_codes
    li      t0 0 # t0 = i, contacting to lines_of_rvc_codes
    addi    a0 s0 -4 # a0 is the temporary pointer of instr[]
    addi    a1 s2 -4 # a1 is the temporary pointer of size[]

Read_codes:
    addi    t0 t0 1 # i++
    bgt     t0 s11 Adjust_offset1 # if i>lines, then exit reading process
    addi    a0 a0 4 # Get the address of instr[i-1]
    addi    a1 a1 4 # Get the address of size[i-1]
    add     s1 s1 a2 # Add offset to the address of rvc_codes
    lhu     t1 0(s1) # t1 is the last 16-bits
    andi    t3 t1 0b11 # Extract the last 2 bits
    li      t4 0b11
    bne     t3 t4 Load_16 # If the last 2 bits are not "11", it indicates a rvc instruction
    # Else it indicates a RV32 instruction. The rest 16 bits should be loaded
    lhu     t2 2(s1) # t2 is the first 16-bits
    # Now concatenate the two 16-bits
    slli    t5 t2 16 # t5 is the higher 16 bits of RV32 instruction
    add     t5 t5 t1 # t5 gets the lower 16 bits of RV32 instruction
    sw      t5 0(a0) # instr[i-1] = t5
    li      t5 4 # The offset of a 32-bit instruction is 4
    sw      t5 0(a1) # Save the offset in size[i-1]
    li      a2 4 # Shift position for 4 bytes, i.e. a word
    j       Read_codes

Load_16:
    # Store t0, ra, a0 and a1 in advance
    addi    sp sp -16
    sw      t0 12(sp)
    sw      ra 8(sp)
    sw      a0 4(sp)
    sw      a1 0(sp)

    mv      a0 t1 # Set a0, the function argument, as the rvc instruction
    jal     Translate # Translate the rvc instruction
    mv      t1 a1 # Give the return value to t1

    # Restore t0, ra, a0 and a1
    lw      a1 0(sp)
    lw      a0 4(sp)
    lw      ra 8(sp)
    lw      t0 12(sp)
    addi    sp sp 16

    sw      t1 0(a0) # instr[i-1] = t1
    li      t1 2 # The offset of a 16-bit instruction is 2
    sw      t1 0(a1) # size[i-1] = 2
    li      a2 2 # Shift position for 2 bytes, i.e. a half word
    j       Read_codes

Translate:
# Note:
# The argument is a0, representing the rvc code
# The return value of this function, the 32-bit code, should be a1

# In this function, we should distinguish the specific type of each rvc instruction

# c.li
    srli    t0 a0 13 # t0 = func3
    andi    t1 a0 0b11 # t1 = opcode
    slli    t0 t0 2 # Reserve space for opcode
    add     t0 t0 t1 # Concatenate func3 and opcode, stored in t0
    xori    t0 t0 0b01001 # Check if the instr is c.li
    beq     t0 x0 Expand_c_li
# c.lui
    srli    t0 a0 13 # t0 = func3
    andi    t1 a0 0b11 # t1 = opcode
    slli    t0 t0 2 # Reserve space for opcode
    add     t0 t0 t1 # Concatenate func3 and opcode, stored in t0
    xori    t0 t0 0b01101 # Check if the instr is c.lui
    beq     t0 x0 Expand_c_lui
# c.addi
    srli    t0 a0 13 # t0 = func3
    andi    t1 a0 0b11 # t1 = opcode
    slli    t0 t0 2 # Reserve space for opcode
    add     t0 t0 t1 # Concatenate func3 and opcode, stored in t0
    xori    t0 t0 0b00001 # Check if the instr is c.addi
    beq     t0 x0 Expand_c_addi
# c.slli
    srli    t0 a0 13 # t0 = func3
    andi    t1 a0 0b11 # t1 = opcode
    slli    t0 t0 2 # Reserve space for opcode
    add     t0 t0 t1 # Concatenate func3 and opcode, stored in t0
    xori    t0 t0 0b00010 # Check if the instr is c.slli
    beq     t0 x0 Expand_c_slli
# c.lw
    srli    t0 a0 13 # t0 = func3
    slli    t0 t0 2 # Reserve space for opcode
    andi    t1 a0 0b11 # t1 = opcode
    add     t0 t0 t1 # Concatenate func3 and opcode
    xori    t0 t0 0b01000 # Check if the instr is c.lw
    beq     t0 x0 Expand_c_lw
# c.sw
    srli    t0 a0 13 # t0 = func3
    slli    t0 t0 2 # Reserve space for opcode
    andi    t1 a0 0b11 # t1 = opcode
    add     t0 t0 t1 # Concatenate func3 and opcode
    xori    t0 t0 0b11000 # Check if the instr is c.sw
    beq     t0 x0 Expand_c_sw
# c.and
    srli    t0 a0 10 # t0 = funct6
    andi    t1 a0 0b1100000 # t1 = func2
    srli    t1 t1 3 # Eliminate the subsequent 0's, but reserve space for opcode
    andi    t2 a0 0b11 # t2 = opcode
    slli    t0 t0 4 # Reserve space for func2 and opcode
    add     t0 t0 t1 # Concatenate func6 and func2
    add     t0 t0 t2 # Concatenate func6, func2 and opcode
    xori    t0 t0 0b1000111101 # Check if the instr is c.and
    beq     t0 x0 Expand_c_and
# c.or
    srli    t0 a0 10 # t0 = funct6
    andi    t1 a0 0b1100000 # t1 = func2
    srli    t1 t1 3 # Eliminate the subsequent 0's, but reserve space for opcode
    andi    t2 a0 0b11 # t2 = opcode
    slli    t0 t0 4 # Reserve space for func2 and opcode
    add     t0 t0 t1 # Concatenate func6 and func2
    add     t0 t0 t2 # Concatenate func6, func2 and opcode
    xori    t0 t0 0b1000111001 # Check if the instr is c.or
    beq     t0 x0 Expand_c_or
# c.xor
    srli    t0 a0 10 # t0 = funct6
    andi    t1 a0 0b1100000 # t1 = func2
    srli    t1 t1 3 # Eliminate the subsequent 0's, but reserve space for opcode
    andi    t2 a0 0b11 # t2 = opcode
    slli    t0 t0 4 # Reserve space for func2 and opcode
    add     t0 t0 t1 # Concatenate func6 and func2
    add     t0 t0 t2 # Concatenate func6, func2 and opcode
    xori    t0 t0 0b1000110101 # Check if the instr is c.xor
    beq     t0 x0 Expand_c_xor
# c.sub
    srli    t0 a0 10 # t0 = funct6
    andi    t1 a0 0b1100000 # t1 = func2
    srli    t1 t1 3 # Eliminate the subsequent 0's, but reserve space for opcode
    andi    t2 a0 0b11 # t2 = opcode
    slli    t0 t0 4 # Reserve space for func2 and opcode
    add     t0 t0 t1 # Concatenate func6 and func2
    add     t0 t0 t2 # Concatenate func6, func2 and opcode
    xori    t0 t0 0b1000110001 # Check if the instr is c.sub
    beq     t0 x0 Expand_c_sub
# c.beqz
    srli    t0 a0 13 # t0 = func3
    slli    t0 t0 2 # Reserve space for opcode
    andi    t1 a0 0b11 # t1 = opcode
    add     t0 t0 t1 # Concatenate func3 and opcode
    xori    t0 t0 0b11001 # Check if the instr is c.beqz
    beq     t0 x0 Expand_c_beqz
# c.bnez
    srli    t0 a0 13 # t0 = func3
    slli    t0 t0 2 # Reserve space for opcode
    andi    t1 a0 0b11 # t1 = opcode
    add     t0 t0 t1 # Concatenate func3 and opcode
    xori    t0 t0 0b11101 # Check if the instr is c.bnez
    beq     t0 x0 Expand_c_bnez
# c.srli
    srli    t0 a0 13 # t0 = func3
    slli    t0 t0 4 # Reserve space for func2 and opcode
    srli    t1 a0 10
    andi    t1 t1 0b11 # t1 = func2
    slli    t1 t1 2 # Reserve space for opcode
    andi    t2 a0 0b11 # t2 = opcode
    add     t0 t0 t1 # Concatenate func3 and func2
    add     t0 t0 t2 # Concatenate func3, func2 and opcode
    xori    t0 t0 0b1000001 # Check if the instr is c.srli
    beq     t0 x0 Expand_c_srli
# c.srai
    srli    t0 a0 13 # t0 = func3
    slli    t0 t0 4 # Reserve space for func2 and opcode
    srli    t1 a0 10
    andi    t1 t1 0b11 # t1 = func2
    slli    t1 t1 2 # Reserve space for opcode
    andi    t2 a0 0b11 # t2 = opcode
    add     t0 t0 t1 # Concatenate func3 and func2
    add     t0 t0 t2 # Concatenate func3, func2 and opcode
    xori    t0 t0 0b1000101 # Check if the instr is c.srai
    beq     t0 x0 Expand_c_srai
# c.andi
    srli    t0 a0 13 # t0 = func3
    slli    t0 t0 4 # Reserve space for func2 and opcode
    srli    t1 a0 10
    andi    t1 t1 0b11 # t1 = func2
    slli    t1 t1 2 # Reserve space for opcode
    andi    t2 a0 0b11 # t2 = opcode
    add     t0 t0 t1 # Concatenate func3 and func2
    add     t0 t0 t2 # Concatenate func3, func2 and opcode
    xori    t0 t0 0b1001001 # Check if the instr is c.andi
    beq     t0 x0 Expand_c_andi
# c.j
    srli    t0 a0 13 # t0 = func3
    slli    t0 t0 2 # Reserve space for opcode
    andi    t1 a0 0b11 # t1 = opcode
    add     t0 t0 t1 # Concatenate func3 and opcode
    xori    t0 t0 0b10101 # Check if the instr is c.j
    beq     t0 x0 Expand_c_j
# c.jal
    srli    t0 a0 13 # t0 = func3
    slli    t0 t0 2 # Reserve space for opcode
    andi    t1 a0 0b11 # t1 = opcode
    add     t0 t0 t1 # Concatenate func3 and opcode
    xori    t0 t0 0b00101 # Check if the instr is c.jal
    beq     t0 x0 Expand_c_jal
# c.add/c.jalr
    srli    t0 a0 12 # t0 = func4
    slli    t0 t0 2 # Reserve space for opcode
    andi    t1 a0 0b11 # t1 = opcode
    add     t0 t0 t1 # Concatenate func4 and opcode
    xori    t0 t0 0b100110 # Check if the instr is c.add/c.jalr
    beq     t0 x0 Identify_add_jalr
# c.mv/c.jr
    srli    t0 a0 12 # t0 = func4
    slli    t0 t0 2 # Reserve space for opcode
    andi    t1 a0 0b11 # t1 = opcode
    add     t0 t0 t1 # Concatenate func4 and opcode
    xori    t0 t0 0b100010 # Check if the instr is c.mv/c.jr
    beq     t0 x0 Identify_mv_jr

Adjust_offset1:
    j       Adjust_offset

Identify_add_jalr:
    andi    t0 a0 0b1111100
    srli    t0 t0 2 # t0 = src
    bne     t0 x0 Expand_c_add # If src!=0, then the instruction is c.add
    j       Expand_c_jalr # Otherwise (src=0) the instruction is c.jalr

Identify_mv_jr:
    andi    t0 a0 0b1111100
    srli    t0 t0 2 # t0 = src
    bne     t0 x0 Expand_c_mv # If src!=0, then the instruction is c.mv
    j       Expand_c_jr # Otherwise (src=0) the instruction is c.jr

Expand_c_add:
    andi    t0 a0 0b1111100
    srli    t0 t0 2 # t0 = rs2
    srli    t1 a0 7
    andi    t1 t1 0b11111 # t1 = rd/rs1
    li      a1 0b0000000 # Load func7
    slli    a1 a1 5 # Reserve space for rs2
    add     a1 a1 t0 # Concatenate func7 with rs2
    slli    a1 a1 5 # Reserve space for rs1
    add     a1 a1 t1 # Concatenate rs1
    slli    a1 a1 8 # Reserve space for func3 and rd
    add     a1 a1 t1 # Concatenate rd
    slli    a1 a1 7 # Reserve space for opcode
    li      t2 0b0110011 # t2 = opcode
    add     a1 a1 t2 # Concatenate opcode
    j       Translate_exit

Expand_c_mv:
    andi    t0 a0 0b1111100
    srli    t0 t0 2 # t0 = rs2
    srli    t1 a0 7
    andi    t1 t1 0b11111 # t1 = rd, rs1 = 0
    li      a1 0b0000000 # Load func7
    slli    a1 a1 5 # Reserve space for rs2
    add     a1 a1 t0 # Concatenate func7 with rs2
    slli    a1 a1 8 # Reserve space for rs1 and func3
    slli    a1 a1 5 # Reserve space for rd
    add     a1 a1 t1 # Concatenate rd
    slli    a1 a1 7 # Reserve space for opcode
    li      t2 0b0110011 # t2 = opcode
    add     a1 a1 t2 # Concatenate opcode
    j       Translate_exit

Expand_c_jr:
    srli    t0 a0 7
    andi    t0 t0 0b11111# t0=rs1, rd=0
    li      a1 0b000000000000 # load immediate
    slli    a1 a1 5 # Reserve space for rs1
    add     a1 a1 t0 # add rs1
    slli    a1 a1 3 # Reserve space for func3
    addi    a1 a1 0b000 # add func3
    slli    a1 a1 5 # Reserve space for rd
    addi    a1 a1 0b00000 # add rd
    slli    a1 a1 7 # Reserve space for opcode
    addi    a1 a1 0b1100111 # add opcode
    j       Translate_exit 

Expand_c_jalr:
    srli    t0 a0 7
    andi    t0 t0 0b11111 # t0=rs1, rd = x1
    li      a1 0b000000000000 # load immediate
    slli    a1 a1 5 # Reserve space for rs1
    add     a1 a1 t0 # add rs1
    slli    a1 a1 3 # Reserve space for func3
    addi    a1 a1 0b000 # add func3
    slli    a1 a1 5 # Reserve space for rd
    addi    a1 a1 0b00001 # add rd
    slli    a1 a1 7 # Reserve space for opcode
    addi    a1 a1 0b1100111 # add opcode
    j       Translate_exit

Expand_c_li:
    srli    t0 a0 12 
    andi    t0 t0 0b1 # load imm[5]
    slli    t0 t0 5 # Reserve space for imm[4:0]
    andi    t1 a0 0b1111100 # load imm[4:0]
    srli    t1 t1 2
    add     t0 t0 t1 # t0=imm[5:0]
    srli    t1 a0 7
    andi    t1 t1 0b11111 # t1=rd, rs1=0
    mv      a1 t0 # a1=imm[5:0]
    slli    a1 a1 26
    srai    a1 a1 26 # Sign-extended for higher bits
    slli    a1 a1 5 # add rs1
    slli    a1 a1 3 # Reserve space for func3
    addi    a1 a1 0b000 # add func3
    slli    a1 a1 5 # Reserve space for rd
    add     a1 a1 t1 # add rd
    slli    a1 a1 7 # Reserve space for opcode
    addi    a1 a1 0b0010011
    j       Translate_exit

Expand_c_lui:
    srli    t0 a0 12 
    andi    t0 t0 0b1 # load imm[17]
    slli    t0 t0 5 # Reserve space for imm[16:12]
    andi    t1 a0 0b1111100 # load imm[16:12]
    srli    t1 t1 2
    add     t0 t0 t1 # t0=imm[17:12]
    srli    t1 a0 7
    andi    t1 t1 0b11111 # t1=rs1=rd
    mv      a1 t0 # a1=imm[17:12]
    slli    a1 a1 26
    srai    a1 a1 26 # Sign-extended for higher bits
    slli    a1 a1 5 # Reserve space for rd
    add     a1 a1 t1 # add rd
    slli    a1 a1 7 # Reserve space for opcode
    addi    a1 a1 0b0110111
    j       Translate_exit

Expand_c_addi:
    srli    t0 a0 12 
    andi    t0 t0 0b1 # load imm[5]
    slli    t0 t0 5 # Reserve space for imm[4:0]
    andi    t1 a0 0b1111100 # load imm[4:0]
    srli    t1 t1 2
    add     t0 t0 t1 # t0=imm[5:0]
    srli    t1 a0 7
    andi    t1 t1 0b11111 # t1=rs1=rd
    mv      a1 t0 # a1=imm
    slli    a1 a1 26
    srai    a1 a1 26 # Sign-extended for higher bits
    slli    a1 a1 5 # Reserve space for rs1
    add     a1 a1 t1 # add rs1
    slli    a1 a1 3 # Reserve space for func3
    addi    a1 a1 0b000 # add func3
    slli    a1 a1 5 # Reserve space for rd
    add     a1 a1 t1 # add rd
    slli    a1 a1 7 # Reserve space for opcode
    addi    a1 a1 0b0010011
    j       Translate_exit

Expand_c_slli:
    andi    t0 a0 0b1111100 # load shamt[4:0]
    srli    t0 t0 2 # t0=shamt[4:0]
    srli    t1 a0 7
    andi    t1 t1 0b11111 # t1=rs1=rd
    li      a1 0b0000000 # a1 = seven leading 0's
    slli    a1 a1 5 # Reserve space for shamt
    add     a1 a1 t0 # add shamt
    slli    a1 a1 5 # Reserve space for rs1
    add     a1 a1 t1 # add rs1
    slli    a1 a1 3 # Reserve space for func3
    addi    a1 a1 0b001 # add func3
    slli    a1 a1 5 # Reserve space for rd
    add     a1 a1 t1 # add rd
    slli    a1 a1 7 # Reserve space for opcode
    addi    a1 a1 0b0010011
    j       Translate_exit

Expand_c_lw:
    andi    t0 a0 0b100000 # load offset[6]
    srli    t0 t0 2 # Reserve space for offset[5:3]
    srli    t1 a0 10
    andi    t1 t1 0b111 # load offset[5:3]
    add     t0 t0 t1 # Add offset[5:3]
    slli    t0 t0 1 # Reserve space for offset[2]
    andi    t1 a0 0b1000000
    srli    t1 t1 6 # load from offset[2]
    add     t0 t0 t1 # Add offset[2],t0=offset[6:2]
    andi    t1 a0 0b1110000000 
    srli    t1 t1 7 # t1=rs1'
    andi    t2 a0 0b11100
    srli    t2 t2 2 # t2=rd'
    mv      a1 t0
    slli    a1 a1 2 # t0=offset[6:0]
    slli    a1 a1 5 # Reserve space for rs1
    add     a1 a1 t1 # add rs1' 
    addi    a1 a1 8 # update rs1    
    slli    a1 a1 3 # Reserve space for funct3
    addi    a1 a1 0b010 # add funct3
    slli    a1 a1 5 # Reserve space for rd
    add     a1 a1 t2 # add rd'
    addi    a1 a1 8 # update rd
    slli    a1 a1 7 # Reserve space for opcode
    addi    a1 a1 0b0000011 # add opcode
    j       Translate_exit

Expand_c_sw:
    andi    t0 a0 0b100000 # load offset[6]
    srli    t0 t0 2 # Reserve space for offset[5:3]
    srli    t1 a0 10
    andi    t1 t1 0b111 # load offset[5:3]
    add     t0 t0 t1 # Add offset[5:3]
    slli    t0 t0 1 # Reserve space for offset[2]
    andi    t1 a0 0b1000000
    srli    t1 t1 6 # load from offset[2]
    add     t0 t0 t1 # Add offset[2],t0=offset[6:2]
    slli    t0 t0 2 # t0=offset[6:0]
    andi    t1 a0 0b1110000000 
    srli    t1 t1 7 # t1=rs1'
    andi    t2 a0 0b11100
    srli    t2 t2 2 # t2=rs2'
    srli    t3 t0 5
    andi    t3 t3 0b1111111# t3=offset[11:5]
    andi    t0 t0 0b11111 # t0=offset[4:0]
    mv      a1 t3 # add offset[11:5]
    slli    a1 a1 5 # Reserve space for rs2
    add     a1 a1 t2 # add rs2' 
    addi    a1 a1 8 # update rs2
    slli    a1 a1 5 # Reserve space for rs1
    add     a1 a1 t1 # add rs1'
    addi    a1 a1 8 # update rs1    
    slli    a1 a1 3 # Reserve space for funct3
    addi    a1 a1 0b010 # add funct3
    slli    a1 a1 5 # Reserve space for imm[4:0]
    add     a1 a1 t0
    slli    a1 a1 7 # Reserve space for opcode
    addi    a1 a1 0b0100011 # add opcode
    j       Translate_exit

Expand_c_and:
    andi    t0 a0 0b1110000000 # load rs1'
    srli    t0 t0 7 # t0=rd'=rs1'
    andi    t1 a0 0b11100 # load rs2'
    srli    t1 t1 2 # t1=rs2'
    li      a1 0b0000000 # add funct7
    slli    a1 a1 5 # Reserve space for rs2'
    add     a1 a1 t1 # add rs2'
    addi    a1 a1 8 # update rs2
    slli    a1 a1 5 # Reserve space for rs1'
    add     a1 a1 t0 # add rs1'
    addi    a1 a1 8 # update rs1
    slli    a1 a1 3 # Reserve space for funct3
    addi    a1 a1 0b111 # add funct3
    slli    a1 a1 5 # Reserve space for rd'
    add     a1 a1 t0 # add rd'
    addi    a1 a1 8 # update rd
    slli    a1 a1 7 # Reserve space for opcode
    addi    a1 a1 0b0110011 # update opcode
    j       Translate_exit

Expand_c_or:
    andi    t0 a0 0b1110000000 # load rs1'
    srli    t0 t0 7 # t0=rd'=rs1'
    andi    t1 a0 0b11100 # load rs2'
    srli    t1 t1 2 # t1=rs2'
    li      a1 0b0000000 # add funct7
    slli    a1 a1 5 # Reserve space for rs2'
    add     a1 a1 t1 # add rs2'
    addi    a1 a1 8 # update rs2
    slli    a1 a1 5 # Reserve space for rs1'
    add     a1 a1 t0 # add rs1'
    addi    a1 a1 8 # update rs1
    slli    a1 a1 3 # Reserve space for funct3
    addi    a1 a1 0b110 # add funct3
    slli    a1 a1 5 # Reserve space for rd'
    add     a1 a1 t0 # add rd'
    addi    a1 a1 8 # update rd
    slli    a1 a1 7 # Reserve space for opcode
    addi    a1 a1 0b0110011 # update opcode
    j       Translate_exit

Expand_c_xor:
    andi    t0 a0 0b1110000000 # load rs1'
    srli    t0 t0 7 # t0=rd'=rs1'
    andi    t1 a0 0b11100 # load rs2'
    srli    t1 t1 2 # t1=rs2'
    li      a1 0b0000000 # add funct7
    slli    a1 a1 5 # Reserve space for rs2'
    add     a1 a1 t1 # add rs2'
    addi    a1 a1 8 # update rs2
    slli    a1 a1 5 # Reserve space for rs1'
    add     a1 a1 t0 # add rs1'
    addi    a1 a1 8 # update rs1
    slli    a1 a1 3 # Reserve space for funct3
    addi    a1 a1 0b100 # add funct3
    slli    a1 a1 5 # Reserve space for rd'
    add     a1 a1 t0 # add rd'
    addi    a1 a1 8 # update rd
    slli    a1 a1 7 # Reserve space for opcode
    addi    a1 a1 0b0110011 # update opcode
    j       Translate_exit

Expand_c_sub:
    andi    t0 a0 0b1110000000 # load rs1'
    srli    t0 t0 7 # t0=rd'=rs1'
    andi    t1 a0 0b11100 # load rs2'
    srli    t1 t1 2 # t1=rs2'
    li      a1 0b0100000 # add funct7
    slli    a1 a1 5 # Reserve space for rs2'
    add     a1 a1 t1 # add rs2'
    addi    a1 a1 8 # update rs2
    slli    a1 a1 5 # Reserve space for rs1'
    add     a1 a1 t0 # add rs1'
    addi    a1 a1 8 # update rs1
    slli    a1 a1 3 # Reserve space for funct3
    addi    a1 a1 0b000 # add funct3
    slli    a1 a1 5 # Reserve space for rd'
    add     a1 a1 t0 # add rd'
    addi    a1 a1 8 # update rd
    slli    a1 a1 7 # Reserve space for opcode
    addi    a1 a1 0b0110011 # update opcode
    j       Translate_exit

Expand_c_beqz:
    srli    t0 a0 12 
    andi    t0 t0 0b1 # load offset[8]
    slli    t0 t0 2 # Reserve space for offset[7:6]
    andi    t1 a0 0b1100000 
    srli    t1 t1 5 # load offset[7:6]
    add     t0 t0 t1 # t0=offset[8:6]
    slli    t0 t0 1 # Reserve space for offset[5]
    andi    t1 a0 0b100
    srli    t1 t1 2 # load offset[5]
    add     t0 t0 t1 # t0=offset[8:5]
    slli    t0 t0 2 # Reserve space for offset[4:3]
    srli    t1 a0 10
    andi    t1 t1 0b11 # load offset[4:3]
    add     t0 t0 t1 # t0=offset[8:3]
    slli    t0 t0 2 # Reserve space for offset[2:1]
    andi    t1 a0 0b11000
    srli    t1 t1 3 # load offset[2:1]
    add     t0 t0 t1 # t0=offset[8:1]
    slli    t0 t0 24 
    srai    t0 t0 24 # consider the negative

    andi    t1 a0 0b1110000000
    srli    t1 t1 7 # t1=rs1', rs2'=0

    srli    t2 t0 11
    andi    t2 t2 0b1 # t2=offset[12]
    mv      a1 t2
    slli    a1 a1 6 # Reserve space for offset[10:5]
    andi    t2 t0 0b1111110000 # load offset[10:5]
    srli    t2 t2 4 # t2=offset[10:5]
    add     a1 a1 t2 # add offset[10:5]
    slli    a1 a1 5 # add rs2
    slli    a1 a1 5 # Reserve space for rs1'
    add     a1 a1 t1 # add rs1'
    addi    a1 a1 8 #update rs1
    slli    a1 a1 3 # add funct3
    slli    a1 a1 4 #Reserve space for offset[4:1]
    andi    t2 t0 0b1111 # load offset[4:1]
    add     a1 a1 t2 # add offset[4:1]
    slli    a1 a1 1 #Reserve space for offset[11]
    andi    t2 t0 0b10000000000
    srli    t2 t2 10 # t2=offset[11]
    add     a1 a1 t2 # add offset[11]
    slli    a1 a1 7 # Reserve space for opcode
    addi    a1 a1 0b1100011
    j       Translate_exit

Expand_c_bnez:
    srli    t0 a0 12 
    andi    t0 t0 0b1 # load offset[8]
    slli    t0 t0 2 # Reserve space for offset[7:6]
    andi    t1 a0 0b1100000 
    srli    t1 t1 5 # load offset[7:6]
    add     t0 t0 t1 # t0=offset[8:6]
    slli    t0 t0 1 # Reserve space for offset[5]
    andi    t1 a0 0b100
    srli    t1 t1 2 # load offset[5]
    add     t0 t0 t1 # t0=offset[8:5]
    slli    t0 t0 2 # Reserve space for offset[4:3]
    srli    t1 a0 10
    andi    t1 t1 0b11 # load offset[4:3]
    add     t0 t0 t1 # t0=offset[8:3]
    slli    t0 t0 2 # Reserve space for offset[2:1]
    andi    t1 a0 0b11000
    srli    t1 t1 3 # load offset[2:1]
    add     t0 t0 t1 # t0=offset[8:1]
    slli    t0 t0 24 
    srai    t0 t0 24 # consider the negative

    andi    t1 a0 0b1110000000
    srli    t1 t1 7 # t1=rs1', rs2' = 0

    srli    t2 t0 11
    andi    t2 t2 0b1 # t2=offset[12]
    mv      a1 t2
    slli    a1 a1 6 # Reserve space for offset[10:5]
    andi    t2 t0 0b1111110000 # load offset[10:5]
    srli    t2 t2 4 # t2=offset[10:5]
    add     a1 a1 t2 # add offset[10:5]
    slli    a1 a1 5 # add rs2
    slli    a1 a1 5 # Reserve space for rs1'
    add     a1 a1 t1 # add rs1'
    addi    a1 a1 8 #update rs1
    slli    a1 a1 3 # Reserve space for funct3
    addi    a1 a1 0b001 # add funct3
    slli    a1 a1 4 #Reserve space for offset[4:1]
    andi    t2 t0 0b1111 # load offset[4:1]
    add     a1 a1 t2 # add offset[4:1]
    slli    a1 a1 1 #Reserve space for offset[11]
    andi    t2 t0 0b10000000000
    srli    t2 t2 10 # t2=offset[11]
    add     a1 a1 t2 # add offset[11]
    slli    a1 a1 7 # Reserve space for opcode
    addi    a1 a1 0b1100011
    j       Translate_exit

Expand_c_srli:
    andi    t0 a0 0b1110000000 # load rs1'
    srli    t0 t0 7 # t0=rd'=rs1'
    andi    t1 a0 0b1111100 # load shamt
    srli    t1 t1 2 # t1=shamt[4:0]
    li      a1 0b0000000 # add funct7
    slli    a1 a1 5 # Reserve space for shamt
    add     a1 a1 t1 # add shamt[4:0]
    slli    a1 a1 5 # Reserve space for rs1
    add     a1 a1 t0 # add rs1'
    addi    a1 a1 8 # update rs1
    slli    a1 a1 3 # Reserve space for funct3
    addi    a1 a1 0b101 # add funct3
    slli    a1 a1 5 # Reserve space for rd'
    add     a1 a1 t0 # add rd'
    addi    a1 a1 8 # update rd
    slli    a1 a1 7 # Reserve space for opcode
    addi    a1 a1 0b0010011
    j       Translate_exit

Expand_c_srai:
    andi    t0 a0 0b1110000000 # load rs1'
    srli    t0 t0 7 # t0=rd'=rs1'
    andi    t1 a0 0b1111100 # load shamt
    srli    t1 t1 2 # t1=shamt[4:0]
    li      a1 0b0100000 # add funct7
    slli    a1 a1 5 # Reserve space for shamt
    add     a1 a1 t1 # add shamt[4:0]
    slli    a1 a1 5 # Reserve space for rs1
    add     a1 a1 t0 # add rs1'
    addi    a1 a1 8 # update rs1
    slli    a1 a1 3 # Reserve space for funct3
    addi    a1 a1 0b101 # add funct3
    slli    a1 a1 5 # Reserve space for rd'
    add     a1 a1 t0 # add rd'
    addi    a1 a1 8 # update rd
    slli    a1 a1 7 # Reserve space for opcode
    addi    a1 a1 0b0010011
    j       Translate_exit

Expand_c_andi:
    srli    t0 a0 12 # load imm[5]
    andi    t0 t0 0b1 # t0=imm[5]
    slli    t0 t0 5 # Reserve space for imm[4:0]
    andi    t1 a0 0b1111100 # load imm
    srli    t1 t1 2 # t1=imm[4:0]
    add     t0 t0 t1 # t0=imm[5:0]
    slli    t0 t0 26
    srai    t0 t0 26 # consider signed number
    andi    t2 a0 0b1110000000 # load rs1'
    srli    t2 t2 7 # t2=rd'=rs1'
    mv      a1 t0    
    slli    a1 a1 5 # Reserve space for rs1
    add     a1 a1 t2 # add rs1'
    addi    a1 a1 8 # update rs1
    slli    a1 a1 3 # Reserve space for funct3
    addi    a1 a1 0b111 # add funct3
    slli    a1 a1 5 # Reserve space for rd
    add     a1 a1 t2 # add rd'
    addi    a1 a1 8 # update rd
    slli    a1 a1 7 # Reserve space for opcode
    addi    a1 a1 0b0010011 # add opcode
    j       Translate_exit

Expand_c_j:
    srli    t0 a0 12
    andi    t0 t0 0b1 # load offset[11]
    andi    t1 a0 0b100000000
    srli    t1 t1 8 # t1=offset[10]
    slli    t0 t0 1 # Reserve space for offset[10]
    add     t0 t0 t1 # t0=offset[11:10]
    andi    t1 a0 0b11000000000
    srli    t1 t1 9 # t1=offset[9:8]
    slli    t0 t0 2 # Reserve space for offset[9:8]
    add     t0 t0 t1 # t0=offset[11:8]
    andi    t1 a0 0b1000000
    srli    t1 t1 6 # t1=offset[7]
    slli    t0 t0 1 # Reserve space for offset[7]
    add     t0 t0 t1 # t0=offset[11:7]
    andi    t1 a0 0b10000000
    srli    t1 t1 7 # t1=offset[6]
    slli    t0 t0 1 # Reserve space for offset[6]
    add     t0 t0 t1 # t0=offset[6]
    andi    t1 a0 0b100
    srli    t1 t1 2 # t1=offset[5]
    slli    t0 t0 1 # t0=offset[5]
    add     t0 t0 t1 # t0=offset[11:5]
    srli    t1 a0 11
    andi    t1 t1 0b1 # t1=offset[4]
    slli    t0 t0 1 # Reserve space for offset[4]
    add     t0 t0 t1 # t0=offset[11:4]
    andi    t1 a0 0b111000
    srli    t1 t1 3 # t1=offset[3:1]
    slli    t0 t0 3 # Reserve space for offset[3:1]
    add     t0 t0 t1 # t0=offset[11:1]
    slli    t0 t0 21
    srai    t0 t0 21 # consider the signed number

    srli    a1 t0 19
    andi    a1 a1 0b1 # a1=imm[20]
    andi    t1 t0 0b1111111111
    slli    a1 a1 10 # Reserve space for imm[10:1]
    add     a1 a1 t1 # add imm[10:1]
    andi    t1 t0 0b10000000000
    srli    t1 t1 10 # t1=imm[11]
    slli    a1 a1 1 # Reserve space for imm[11]
    add     a1 a1 t1 # add imm[11]
    srli    t1 t0 11
    andi    t1 t1 0b11111111 # t1=imm[19:12]
    slli    a1 a1 8 # Reserve space for imm[19:12]
    add     a1 a1 t1 # add imm[19:12]
    slli    a1 a1 5 # Reserve space for rd
    slli    a1 a1 7 # Reserve space for opcode
    addi    a1 a1 0b1101111 # add opcode
    j       Translate_exit

Expand_c_jal:
    srli    t0 a0 12
    andi    t0 t0 0b1 # load offset[11]
    andi    t1 a0 0b100000000
    srli    t1 t1 8 # t1=offset[10]
    slli    t0 t0 1 # Reserve space for offset[10]
    add     t0 t0 t1 # t0=offset[11:10]
    andi    t1 a0 0b11000000000
    srli    t1 t1 9 # t1=offset[9:8]
    slli    t0 t0 2 # Reserve space for offset[9:8]
    add     t0 t0 t1 # t0=offset[11:8]
    andi    t1 a0 0b1000000
    srli    t1 t1 6 # t1=offset[7]
    slli    t0 t0 1 # Reserve space for offset[7]
    add     t0 t0 t1 # t0=offset[11:7]
    andi    t1 a0 0b10000000
    srli    t1 t1 7 # t1=offset[6]
    slli    t0 t0 1 # Reserve space for offset[6]
    add     t0 t0 t1 # t0=offset[6]
    andi    t1 a0 0b100
    srli    t1 t1 2 # t1=offset[5]
    slli    t0 t0 1 # t0=offset[5]
    add     t0 t0 t1 # t0=offset[11:5]
    srli    t1 a0 11
    andi    t1 t1 0b1
     # t1=offset[4]
    slli    t0 t0 1 # Reserve space for offset[4]
    add     t0 t0 t1 # t0=offset[11:4]
    andi    t1 a0 0b111000
    srli    t1 t1 3 # t1=offset[3:1]
    slli    t0 t0 3 # Reserve space for offset[3:1]
    add     t0 t0 t1 # t0=offset[11:1]
    slli    t0 t0 21
    srai    t0 t0 21 # consider the signed number

    srli    a1 t0 19
    andi    a1 a1 0b1 # a1=imm[20]
    andi    t1 t0 0b1111111111
    slli    a1 a1 10 # Reserve space for imm[10:1]
    add     a1 a1 t1 # add imm[10:1]
    andi    t1 t0 0b10000000000
    srli    t1 t1 10 # t1=imm[11]
    slli    a1 a1 1 # Reserve space for imm[11]
    add     a1 a1 t1 # add imm[11]
    srli    t1 t0 11
    andi    t1 t1 0b11111111 # t1=imm[19:12]
    slli    a1 a1 8 # Reserve space for imm[19:12]
    add     a1 a1 t1 # add imm[19:12]
    slli    a1 a1 5 # Reserve space for rd
    addi    a1 a1 0b00001 # add rd
    slli    a1 a1 7 # Reserve space for opcode
    addi    a1 a1 0b1101111 # add opcode
    j       Translate_exit

Translate_exit:
    jr      ra

Adjust_offset:
    addi    a0 s0 -4 # a0 is the temporary pointer of instr[]
    li      s1 0 # s1 = i

Adjust_lines:
    addi    s1 s1 1 # i++
    bgt     s1 s11 Adjust_exit # if i>lines, then exit adjusting process
    addi    a0 a0 4 # get the address of instr[i-1]
    lhu     t2 0(a0) # load the last 16 bits
    lhu     t1 2(a0) # load the higher 16 bits
    slli    t1 t1 16
    add     t1 t1 t2 # t1 is the word
    mv      s3 t1 # s3 is the 32-bit instruction
    andi    t2 t1 0b1111111 # get the opcode of instr[i-1]
    li      t3 0b1101111 # the opcode of jal
    beq     t2 t3 Adjust_jal # if the opcode is jal , go to jal
    li      t3 0b1100011 # the opcode of B-type
    beq     t2 t3 Adjust_B # if the opcode is B-type, go to B-type
    j       Adjust_lines

Adjust_jal:
    # Do not modify s1
    # s3 represents the 32-bit instruction
    # The updated instruction has to be saved to 0(a0)

    # Extract imm to determine initial offset
    srli    t0 s3 31 # t0 = imm[20]
    srli    t1 s3 12
    andi    t1 t1 0b11111111 # t1 = imm[19:12]
    srli    t2 s3 20
    andi    t2 t2 0b1 # t2 = imm[11]
    srli    t3 s3 21
    andi    t3 t3 0b1111111111 # t3 = imm[10:1]
    slli    t0 t0 8 # Reserve space for imm[19:12]
    add     t0 t0 t1 # t0 = imm[20:12]
    slli    t0 t0 1 # Reserve space for imm[11]
    add     t0 t0 t2 # t0 = imm[20:11]
    slli    t0 t0 10 # Reserve space for imm[10:1]
    add     t0 t0 t3 # t0 = imm[20:1]
    slli    t0 t0 1 # t0 = imm[20:0]
    slli    t0 t0 11
    srai    t0 t0 11 # Sign-extended to higher bits
    mv      a1 t0
    # Now, a1 represents the actual offset, in forms of bytes

    addi    sp sp -4
    sw      ra 0(sp)

    # Goto recalculation function to calculate the true offset
    addi    a3 s1 -1 # a3 is the position of the current instruction, started from 0
    jal     Recalculate_offset

    lw      ra 0(sp)
    addi    sp sp 4

    # Now the true offset is stored by a2
    # Extract rd
    srli    t4 s3 7
    andi    t4 t4 0b11111 # t4 = rd
    # Slice the offset into 4 parts
    srli    a2 a2 1 # Eliminate the last 0
    andi    t0 a2 0b1111111111 # t0 = imm[10:1]
    srli    a2 a2 10 # Eliminate imm[10:1]
    andi    t1 a2 0b1 # t1 = imm[11]
    srli    a2 a2 1 # Eliminate imm[11]
    andi    t2 a2 0b11111111 # t2 = imm[19:12]
    srli    a2 a2 8 # Eliminate imm[19:12]
    andi    t3 a2 0b1 # t3 = imm[20]
    # Reload the respective bits into the instruction, stored by a1
    mv      a1 t3 # a1 = imm[20]
    slli    a1 a1 10 # Reserve space for imm[10:1]
    add     a1 a1 t0 # a1 = imm[20|10:1]
    slli    a1 a1 1 # Reserve space for imm[11]
    add     a1 a1 t1 # a1 = imm[20|10:1|11]
    slli    a1 a1 8 # Reserve space for imm[19:12]
    add     a1 a1 t2 # a1 = imm[20|10:1|11|19:12]
    slli    a1 a1 5 # Reserve space for rd
    add     a1 a1 t4 # Concatenate rd
    slli    a1 a1 7 # Reserve space for opcode
    addi    a1 a1 0b1101111 # Concatenate opcode
    sw      a1 0(a0) # Save the new instruction in instr[i-1]
    j       Adjust_lines

Adjust_B:
    # Do not modify s1
    # s3 represents the 32-bit instruction
    # The updated instruction has to be saved to 0(a0)

    # Successively extract each component of the instruction
    srli    s3 s3 7 # Eliminate opcode
    andi    t0 s3 0b1 # t0 = imm[11]
    srli    s3 s3 1 # Eliminate imm[11]
    andi    t1 s3 0b1111 # t1 = imm[4:1]
    srli    s3 s3 4 # Eliminate imm[4:1]
    andi    t6 s3 0b111 # t6 = funct3
    srli    s3 s3 3 # Eliminate funct3
    andi    t2 s3 0b11111 # t2 = rs1
    srli    s3 s3 5 # Eliminate rs1
    andi    t3 s3 0b11111 # t3 = rs2
    srli    s3 s3 5 # Eliminate rs2
    andi    t4 s3 0b111111 # t4 = imm[10:5]
    srli    s3 s3 6 # Eliminate imm[10:5]
    andi    t5 s3 0b1 # t5 = imm[12]
    
    # Concatenate imm[]
    slli    t5 t5 1 # Reserve space for imm[11]
    add     t5 t5 t0 # t5 = imm[12:11]
    slli    t5 t5 6 # Reserve space for imm[10:5]
    add     t5 t5 t4 # t5 = imm[12:5]
    slli    t5 t5 4 # Reserve space for imm[4:1]
    add     t5 t5 t1 # t5 = imm[12:1]
    slli    t5 t5 1 # t5 = imm[12:0]
    slli    t5 t5 19
    srai    t5 t5 19 # Sign-extended to higher bits
    mv      a1 t5
    # Now, a1 represents the actual offset, in forms of bytes

    addi    sp sp -12
    sw      t2 8(sp)
    sw      t3 4(sp)
    sw      ra 0(sp)

    # Goto recalculation function to calculate the true offset
    addi    a3 s1 -1 # a3 is the position of the current instruction, started from 0
    jal     Recalculate_offset

    lw      ra 0(sp)
    lw      t3 4(sp)
    lw      t2 8(sp)
    addi    sp sp 12

    # Now the true offset is stored by a2
    # Slice the offset into 4 parts
    srli    a2 a2 1 # Eliminate the last 0
    andi    t0 a2 0b1111 # t0 = imm[4:1]
    srli    a2 a2 4 # Eliminate imm[10:1]
    andi    t1 a2 0b111111 # t1 = imm[10:5]
    srli    a2 a2 6 # Eliminate imm[10:5]
    andi    t4 a2 0b1 # t4 = imm[11]
    srli    a2 a2 1 # Eliminate imm[11]
    andi    t5 a2 0b1 # t5 = imm[12]

    # Remember that t2 = rs1, t3 = rs2
    # Reload the respective bits into the instruction, stored by a1
    mv      a1 t5 # a1 = imm[12]
    slli    a1 a1 6 # Reserve space for imm[10:5]
    add     a1 a1 t1 # Concatenate imm[10:5]
    slli    a1 a1 5 # Reserve space for rs2
    add     a1 a1 t3 # Concatenate rs2
    slli    a1 a1 5 # Reserve space for rs1
    add     a1 a1 t2 # Concatenate rs1
    slli    a1 a1 3 # Reserve space for funct3

    addi    sp sp -4
    sw      ra 0(sp)
    jal     Identify_funct3
    lw      ra 0(sp)
    addi    sp sp 4

    add     a1 a1 a6 # Concatenate funct3
    slli    a1 a1 4 # Reserve space for imm[4:1]
    add     a1 a1 t0 # Concatenate imm[4:1]
    slli    a1 a1 1 # Reserve space for imm[11]
    add     a1 a1 t4 # Concatenate imm[11]
    slli    a1 a1 7 # Reserve space for opcode
    addi    a1 a1 0b1100011 # Concatenate opcode
    sw      a1 0(a0) # Save the new instruction in instr[i-1]

    j       Adjust_lines

Identify_funct3: # The return value of funct3 is a6
    li      s9 0b000 # funct3 of beq
    beq     t6 s9 BEQ
    li      s9 0b001 # funct3 of bne
    beq     t6 s9 BNE
    li      s9 0b100 # funct3 of blt
    beq     t6 s9 BLT
    li      s9 0b101 # funct3 of bge
    beq     t6 s9 BGE
    li      s9 0b110 # funct3 of bltu
    beq     t6 s9 BLTU
    li      s9 0b111 # funct3 of bgeu
    beq     t6 s9 BGEU
BEQ:
    li      a6 0b000
    jr      ra
BNE:
    li      a6 0b001
    jr      ra
BLT:
    li      a6 0b100
    jr      ra
BGE:
    li      a6 0b101
    jr      ra
BLTU:
    li      a6 0b110
    jr      ra
BGEU:
    li      a6 0b111
    jr      ra

Recalculate_offset:
    # The original offset is a1, the position of current instruction is a3
    # The return value, the true offset, is stored by a2
    li      a2 0 # Initialize the answer
    bgt     a1 x0 Downward_iteration # If offset>0, then iterate downward
    blt     a1 x0 Upward_iteration # If offset<0, then iterate upward

Downward_iteration:
    ble     a1 x0 Recalculate_offset_exit
    slli    t0 a3 2 # t0 is the offset of the position with respect to base address
    add     t1 s2 t0 # t1 is the actual current position of the pointer of size[]
    lw      t2 0(t1) # t2 is the size of the current instruction, either 2 or 4
    sub     a1 a1 t2
    addi    a3 a3 1 # position++
    addi    a2 a2 4 # The answer increases by 4
    j       Downward_iteration

Upward_iteration:
    bge     a1 x0 Recalculate_offset_exit
    slli    t0 a3 2 # t0 is the offset of the position with respect to base address
    add     t1 s2 t0 # t1 is the actual current position of the pointer of size[]
    lw      t2 -4(t1) # t2 is the size of the current instruction, either 2 or 4
    add     a1 a1 t2
    addi    a3 a3 -1 # position--
    addi    a2 a2 -4 # The answer decreases by 4
    j       Upward_iteration

Recalculate_offset_exit:
    jr      ra

Adjust_exit:
    li      t0 0 # t0 = i = 0

Print:
    bge     t0 s11 Program_exit # If i >= lines_of_rvc_codes, then execute exit function
    slli    t1 t0 2 # t1 = t0*4
    add     a0 s0 t1 # a0 = s0+t1, i.e. the base address + offset (in form of byte)
    lw      a5 0(a0) # a5 = instr[i]
    li      a6 0x80000000
    li      a7 31 # The shift amount of the abstracted bit
    jal     Print_one_line
    addi    t0 t0 1 # i++
    # Print '\n'
    li      a1 10
    li      a0 11
    ecall
    j       Print

Print_one_line:
    beq     a6 x0 Print_return # If all bits have been output, then exit the function
    and     a1 a5 a6 # a1 = the temporarily most significant bit of instr[i], including the subsequent 0's
    srl     a1 a1 a7 # Shift a1 right-wards to eliminate the subsequent 0's
    # Print the temporarily most significant bit
    li      a0 1
    ecall
    srli    a6 a6 1
    addi    a7 a7 -1
    j       Print_one_line

Print_return:
    jr      ra

Program_exit:
    # Exit with error code 0
    mv      a1 x0
    li      a0 17
    ecall