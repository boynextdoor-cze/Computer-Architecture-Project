#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "utils.h"
/*a pow function*/
int power(int base, int n)
{
    int ans = 1; /*define the return*/
    while (n)
    {
        if (n & 1) /*if satisfies*/
            ans *= base;
        base *= base;
        n >>= 1; /*shift right*/
    }
    return ans; /*return the value*/
}

int decimal(char binary[], int first, int last)
{
    /* first indicates the beginning index, and last indicates the end index*/
    int i = 0;
    int ans = 0; /*define the return*/
    for (i = last; i >= first; i--)
    {
        ans += (binary[i] - '0') * power(2, last - i); /*add one by one*/
    }
    return ans; /*return the value*/
}

/* Translate a twos-compliment into a decimal number */
int sign_decimal(char binary[])
{
    int len = strlen(binary);
    int ans = 0, i;
    /* Calculate the non-signed digits */
    for (i = len - 1; i >= 1; --i)
        ans += (binary[i] - '0') * power(2, len - i - 1);
    /* Calculate the signed digit */
    ans -= (binary[0] - '0') * power(2, len - 1);
    return ans;
}

/* Convert a decimal number "num" to a binary string with len="size" */
char *decimal_to_binary(int num, int size)
{
    /* bin is the answer, and tmp[] serves for temporary container */
    char tmp[15];
    char *bin = (char *)calloc(15, sizeof(char));
    /* lsb is the least significant bit of num in binary form, len is the length of binary form */
    int lsb, len = 0, i;
    int flag = 0; /*check the negative*/
    if (num < 0)
    {
        /*if num is negative,change it to positive*/
        flag = 1;
        num = (-1) * num;
    }
    while (num)
    {
        /* Take the least significant bit of num */
        lsb = num & 1;
        /* Store the least significant bit */
        tmp[len++] = lsb + '0';
        num >>= 1;
    }
    /* Fill the rest bits by '0'*/
    for (i = len; i < size; i++)
        tmp[i] = '0';
    /* Since tmp[] stores the reversed binary code of num, we should reverse it to the correct order */
    for (i = 0; i < size; ++i)
        *(bin + i) = tmp[size - i - 1];
    if (flag == 0)
        return bin;
    /* As for negative number, firstly negate each bit */
    for (i = 0; i < size; ++i)
    {
        if (bin[i] == '0')/*if it is 0*/
            bin[i] = '1';
        else/*if it is 1*/
            bin[i] = '0';
    }
    /* Then add 1 */
    for (i = size - 1; i >= 0; i--)
    {
        if (flag == 1 && bin[i] == '0')/*simply add*/
        {
            bin[i] = '1';
            flag = 0;
        }
        else if (flag == 1 && bin[i] == '1')/*add and add the next*/
            bin[i] = '0';
    }
    return bin; /*return the value*/
}

char type_determine(char binary[])
{
    int flag = decimal(binary, 25, 31);
    switch (flag) /*consider the flag*/
    {
    case 51: /*add...*/
        return 'R';
    case 59: /*addw...*/
        return 'R';
    case 19: /*addi...*/
        return 'I';
    case 3: /*lb...*/
        return 'I';
    case 27: /*addiw...*/
        return 'I';
    case 103: /*jalr*/
        return 'I';
    case 115: /*ecall ebreak*/
        return 'I';
    case 35: /*sb...*/
        return 'S';
    case 99: /*beq...*/
        return 'B';
    case 23: /*auipc*/
        return 'U';
    case 55: /*lui*/
        return 'U';
    case 111: /*jal*/
        return 'J';
    default: /*check for invalid*/
        printf("This is invalid.");
        return 0;
    }
}
/*determine the possible 32-name*/
char *name_32_determine(char type, char binary[])
{
    char opcode[10] = {0}, func3[10] = {0}, func7[10] = {0}, *name_32;
    /*save opcode,func3,func7*/
    strncpy(opcode, binary + 25, 7);/*copy opcode*/
    strncpy(func3, binary + 17, 3);/*copy func3*/
    strncpy(func7, binary, 7);
    /*require some space for the return value*/
    name_32 = (char *)malloc(sizeof(char) * 7);
    if (type == 'R' && !strcmp(opcode, "0110011"))
    {
        /*type add*/
        if (!strcmp(func3, "000") && !strcmp(func7, "0000000"))
        {
            strcpy(name_32, "add");
            return name_32;/*return name*/
        }
        /*type sub*/
        if (!strcmp(func3, "000") && !strcmp(func7, "0100000"))
        {
            strcpy(name_32, "sub");
            return name_32;/*return name*/
        }
        /*type xor*/
        if (!strcmp(func3, "100") && !strcmp(func7, "0000000"))
        {
            strcpy(name_32, "xor");
            return name_32;/*return name*/
        }
        /*type or*/
        if (!strcmp(func3, "110") && !strcmp(func7, "0000000"))
        {
            strcpy(name_32, "or");
            return name_32;/*return name*/
        }
        /*type and*/
        if (!strcmp(func3, "111") && !strcmp(func7, "0000000"))
        {
            strcpy(name_32, "and");
            return name_32;/*return name*/
        }
    }
    /*format I*/
    else if (type == 'I')
    {
        /*type jalr*/
        if (!strcmp(opcode, "1100111"))
        {
            strcpy(name_32, "jalr");
            return name_32;/*return name*/
        }
        /*other type*/
        else if (!strcmp(opcode, "0010011"))
        {
            /*type addi*/
            if (!strcmp(func3, "000"))
            {
                strcpy(name_32, "addi");
                return name_32;/*return name*/
            }
            /*type slli*/
            else if (!strcmp(func3, "001"))
            {
                strcpy(name_32, "slli");
                return name_32;/*return name*/
            }
            /*type srli*/
            else if (!strcmp(func3, "101") && !strcmp(func7, "0000000"))
            {
                strcpy(name_32, "srli");
                return name_32;/*return name*/
            }
            /*type srai*/
            else if (!strcmp(func3, "101") && !strcmp(func7, "0100000"))
            {
                strcpy(name_32, "srai");
                return name_32;/*return name*/
            }
            /*type andi*/
            else if (!strcmp(func3, "111"))
            {
                strcpy(name_32, "andi");
                return name_32;/*return name*/
            }
        }
        /*other type*/
        else if (!strcmp(opcode, "0000011"))
        {
            /*type lw*/
            if (!strcmp(func3, "010"))
            {
                strcpy(name_32, "lw");
                return name_32;/*return name*/
            }
        }
    }
    /*format U*/
    else if (type == 'U' && !strcmp(opcode, "0110111"))
    {
        /*type lui*/
        strcpy(name_32, "lui");
        return name_32;/*return name*/
    }
    /*format B*/
    else if (type == 'B')
    {
        /*type beq*/
        if (!strcmp(func3, "000"))
        {
            strcpy(name_32, "beq");
            return name_32;/*return name*/
        }
        /*type bne*/
        else if (!strcmp(func3, "001"))
        {
            strcpy(name_32, "bne");
            return name_32;/*return name*/
        }
    }
    /*format J*/
    else if (type == 'J')
    {
        /*type jal*/
        strcpy(name_32, "jal");
        return name_32;/*return name*/
    }
    /*format S*/
    else if (type == 'S')
    {
        if (!strcmp(func3, "010"))
        {
            strcpy(name_32, "sw"); /*type sw*/
            return name_32;/*return name*/
        }
    }
    /*if satisfies none of the above*/
    strcpy(name_32, "False");
    return name_32;/*return name*/
}

char *name_16_determine(char name_32[], char binary[])
{
    char *name_16;         /*the return value*/
    int rd, rs1, rs2, imm; /*register*/
    char tmp[10] = {0};
    char immed[30] = {0}; /*save immediate*/
    name_16 = (char *)malloc(sizeof(char) * 10);
    if (!strcmp(name_32, "add"))
    {                                  /*type add*/
        rd = decimal(binary, 20, 24);  /*get value rd*/
        rs1 = decimal(binary, 12, 16); /*get value rs1*/
        rs2 = decimal(binary, 7, 11);  /*get value rs2*/
        if (rd != 0 && rs2 != 0 && rd == rs1)
        { /*rd!=0 and rs2!=0 and rd=rs1*/
            strcpy(name_16, "add");
            return name_16; /*return name*/
        }                   /*type mv*/
        else if (rd != 0 && rs2 != 0 && rs1 == 0)
        { /*rd!=0 and rs2!=0 and rs1=0*/
            strcpy(name_16, "mv");
            return name_16; /*return name*/
        }
    }
    else if (!strcmp(name_32, "jalr"))
    {                                  /*type jalr*/
        rd = decimal(binary, 20, 24);  /*get value rd*/
        rs1 = decimal(binary, 12, 16); /*get value rs1*/
        strncpy(immed, binary, 12);    /*get the immediate*/
        imm = sign_decimal(immed);
        if (rd == 0 && rs1 != 0 && imm == 0)
        { /*rd=0 and imm=0 and rs1!=0*/
            strcpy(name_16, "jr");
            return name_16; /*return name*/
        }
        else if (rd == 1 && rs1 != 0 && imm == 0)
        { /*rd=1 and imm=0 and rs1!=0*/
            strcpy(name_16, "jalr");
            return name_16; /*return name*/
        }
    }
    else if (!strcmp(name_32, "addi"))
    {                                  /*type addi*/
        rd = decimal(binary, 20, 24);  /*get value rd*/
        rs1 = decimal(binary, 12, 16); /*get value rs1*/
        strncpy(immed, binary, 12);
        imm = sign_decimal(immed); /*get the immediate*/
        if (imm <= power(2, 5) - 1 && imm >= (-1) * power(2, 5))
        { /*imm is in the range*/
            if (rd != 0 && rs1 == 0)
            { /*rd!=0 and rs1=0*/
                strcpy(name_16, "li");
                return name_16; /*return name*/
            }
            else if (rd != 0 && rd == rs1 && imm != 0)
            {                            /*imm is in the range*/
                strcpy(name_16, "addi"); /*re=rs1 and rd!=0*/
                return name_16;          /*return name*/
            }
        }
    }
    else if (!strcmp(name_32, "lui"))
    {
        rd = decimal(binary, 20, 24); /*get value rd*/
        strncpy(immed, binary, 20);
        imm = sign_decimal(immed); /*get the immediate*/
        if (imm <= power(2, 5) - 1 && imm >= (-1) * power(2, 5) && imm != 0)
        { /*imm is in the range*/
            if (rd != 0 && rd != 2)
            { /*rd!=0 and rd!=2*/
                strcpy(name_16, "lui");
                return name_16; /*return name*/
            }
        }
    }
    else if (!strcmp(name_32, "slli"))
    {                                  /*type slli*/
        rd = decimal(binary, 20, 24);  /*get value rd*/
        rs1 = decimal(binary, 12, 16); /*get value rs1*/
        if (rd == rs1 && rd != 0)
        { /*rd=rs1 and rd!=0*/
            strcpy(name_16, "slli");
            return name_16;/*return name*/
        }
    }
    else if (!strcmp(name_32, "lw"))
    {
        rd = decimal(binary, 20, 24);  /*get value rd*/
        rs1 = decimal(binary, 12, 16); /*get value rs1*/
        strncpy(immed, binary, 12);
        imm = sign_decimal(immed); /*get the immediate*/
        if (binary[10] == '0' && binary[11] == '0' && imm >= 0)
        { /*the least two significant bits are zero*/
            if (rd >= 8 && rd <= 15 && rs1 >= 8 && rs1 <= 15)
            { /*check register*/
                if (imm <= power(2, 7) - 1)
                { /*imm is in the range*/
                    strcpy(name_16, "lw");
                    return name_16; /*return name*/
                }
            }
        }
    }
    else if (!strcmp(name_32, "sw"))
    {
        rs1 = decimal(binary, 12, 16); /*get value rs1*/
        rs2 = decimal(binary, 7, 11);  /*get value rs2*/
        strncpy(tmp, binary + 20, 5);
        strncpy(immed, binary, 7); /*get the immediate*/
        strcat(immed, tmp);        /*get the immediate*/
        imm = sign_decimal(immed);
        if (binary[24] == '0' && binary[23] == '0' && imm >= 0)
        {
            if (rs1 >= 8 && rs1 <= 15 && rs2 >= 8 && rs2 <= 15)
            { /*check register*/
                if (imm <= power(2, 7) - 1)
                { /*imm is in the range*/
                    strcpy(name_16, "sw");
                    return name_16; /*return name*/
                }
            }
        }
    }
    else if (!strcmp(name_32, "and"))
    {                                  /*check type and*/
        rd = decimal(binary, 20, 24);  /*get value rd*/
        rs1 = decimal(binary, 12, 16); /*get value rs1*/
        rs2 = decimal(binary, 7, 11);  /*get value rs2*/
        if (rd == rs1 && rd >= 8 && rd <= 15 && rs2 >= 8 && rs2 <= 15)
        { /*check register*/
            strcpy(name_16, "and");
            return name_16; /*return name*/
        }
    }
    else if (!strcmp(name_32, "or"))
    {
        rd = decimal(binary, 20, 24);  /*get value rd*/
        rs1 = decimal(binary, 12, 16); /*get value rs1*/
        rs2 = decimal(binary, 7, 11);  /*get value rs2*/
        if (rd == rs1 && rd >= 8 && rd <= 15 && rs2 >= 8 && rs2 <= 15)
        { /*check register*/
            strcpy(name_16, "or");
            return name_16; /*return name*/
        }
    }
    else if (!strcmp(name_32, "xor"))
    {
        rd = decimal(binary, 20, 24);  /*get value rd*/
        rs1 = decimal(binary, 12, 16); /*get value rs1*/
        rs2 = decimal(binary, 7, 11);  /*get value rs2*/
        if (rd == rs1 && rd >= 8 && rd <= 15 && rs2 >= 8 && rs2 <= 15)
        { /*check register*/
            strcpy(name_16, "xor");
            return name_16; /*return name*/
        }
    }
    else if (!strcmp(name_32, "sub"))
    {
        rd = decimal(binary, 20, 24);  /*get value rd*/
        rs1 = decimal(binary, 12, 16); /*get value rs1*/
        rs2 = decimal(binary, 7, 11);  /*get value rs2*/
        if (rd == rs1 && rd >= 8 && rd <= 15 && rs2 >= 8 && rs2 <= 15)
        {
            strcpy(name_16, "sub");
            return name_16; /*return name*/
        }
    }
    else if (!strcmp(name_32, "beq"))
    {
        rs1 = decimal(binary, 12, 16); /*get value rs1*/
        rs2 = decimal(binary, 7, 11);  /*get value rs2*/
        if (rs2 == 0 && rs1 >= 8 && rs1 <= 15)
        { /*check register*/
            strcpy(name_16, "beqz");
            return name_16; /*return name*/
        }
    }
    else if (!strcmp(name_32, "bne"))
    {
        rs1 = decimal(binary, 12, 16); /*get value rs1*/
        rs2 = decimal(binary, 7, 11);  /*get value rs2*/
        if (rs2 == 0 && rs1 >= 8 && rs1 <= 15)
        { /*check register*/
            strcpy(name_16, "bnez");
            return name_16; /*return name*/
        }
    }
    else if (!strcmp(name_32, "srli"))
    {
        rd = decimal(binary, 20, 24);  /*get value rd*/
        rs1 = decimal(binary, 12, 16); /*get value rs1*/
        if (rd == rs1 && rd >= 8 && rd <= 15)
        { /*check register*/
            strcpy(name_16, "srli");
            return name_16; /*return name*/
        }
    }
    else if (!strcmp(name_32, "srai"))
    {
        rd = decimal(binary, 20, 24);  /*get value rd*/
        rs1 = decimal(binary, 12, 16); /*get value rs1*/
        if (rd == rs1 && rd >= 8 && rd <= 15)
        { /*check register*/
            strcpy(name_16, "srai");
            return name_16; /*return name*/
        }
    }
    /*type andi*/
    else if (!strcmp(name_32, "andi"))
    {
        rd = decimal(binary, 20, 24);  /*get the rd and rs1 register*/
        rs1 = decimal(binary, 12, 16); /*get value rs1*/
        strncpy(immed, binary, 12);    /*get the immediate value*/
        imm = sign_decimal(immed);     /*get the immediate*/
        if (imm <= power(2, 5) - 1 && imm >= (-1) * power(2, 5))
        {
            /*first constrain :imm is in the range*/
            if (rd == rs1 && rd >= 8 && rd <= 15)
            {
                /*second constrain:use register x8-x15*/
                strcpy(name_16, "andi");
                return name_16; /*return name*/
            }
        }
    }
    else if (!strcmp(name_32, "jal"))
    {
        rd = decimal(binary, 20, 24); /*get value rd*/
        if (rd == 0)
        { /*type j*/
            strcpy(name_16, "j");
            return name_16;/*return name*/
        }
        else if (rd == 1)
        { /*type jal*/
            strcpy(name_16, "jal");
            return name_16; /*return name*/
        }
    }
    strcpy(name_16, "False");
    return name_16;/*return name*/
}

char *compress(char name_16[], char binary[])
{
    char *compressed;                                /*the return value*/
    int imm;                                         /*immediate*/
    char rd[10] = {0}, rs1[10] = {0}, rs2[10] = {0}; /*registers*/
    char tmp[15] = {0};
    char immed[30] = {0}; /*save immediate*/
    char *new_imm;
    int reg1_id, reg2_id;    /* The index of x8-x15 */
    char *bit3_rd, *bit3_rs; /* The transferred binary representation of x8-x15 */
    compressed = (char *)malloc(sizeof(char) * 20);
    if (!strcmp(name_16, "add"))
    {
        strncpy(rd, binary + 20, 5); /*get value rd*/
        strncpy(rs2, binary + 7, 5); /*get value rs2*/
        strcpy(compressed, "1001");/*add the prefix*/
        strcat(compressed, rd);/*add rd*/
        strcat(compressed, rs2);/*add rs2*/
        strcat(compressed, "10");/*add last*/
    }
    else if (!strcmp(name_16, "mv"))
    {
        strncpy(rd, binary + 20, 5); /*get value rd*/
        strncpy(rs2, binary + 7, 5); /*get value rs2*/
        strcpy(compressed, "1000");/*add the prefix*/
        strcat(compressed, rd);/*add rd*/
        strcat(compressed, rs2);/*add rs2*/
        strcat(compressed, "10");/*add last*/
    }
    else if (!strcmp(name_16, "jr"))
    {
        strncpy(rs1, binary + 12, 5); /*get value rs1*/
        strcpy(compressed, "1000");/*add the prefix*/
        strcat(compressed, rs1);/*add rs1*/
        strcat(compressed, "00000");
        strcat(compressed, "10");/*add last*/
    }
    else if (!strcmp(name_16, "jalr"))
    {
        strncpy(rs1, binary + 12, 5); /*get value rs1*/
        strcpy(compressed, "1001");/*add the prefix*/
        strcat(compressed, rs1);/*add rs1*/
        strcat(compressed, "00000");
        strcat(compressed, "10");/*add last*/
    }
    else if (!strcmp(name_16, "li"))
    {
        strncpy(rd, binary + 20, 5);/*get rd*/
        strncpy(immed, binary, 12);
        imm = sign_decimal(immed);/*get imm*/
        new_imm = decimal_to_binary(imm, 6);
        /* Save func3 */
        strcpy(compressed, "010");
        /* Save imm[5] */
        strncpy(tmp, new_imm, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save rd */
        strcat(compressed, rd);
        /* Save imm[4:0] */
        strncpy(tmp, new_imm + 1, 5);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save opcode */
        strcat(compressed, "01");
        free(new_imm);
    }
    else if (!strcmp(name_16, "lui"))
    {
        strncpy(rd, binary + 20, 5);/*get rd*/
        strncpy(immed, binary, 20);
        imm = sign_decimal(immed); /*get the immediate*/
        new_imm = decimal_to_binary(imm, 6);
        /* Save func3 */
        strcpy(compressed, "011");
        /* Save nzimm[17] */
        strncpy(tmp, new_imm, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save rd */
        strcat(compressed, rd);
        /* Save nzimm[16:12] */
        strncpy(tmp, new_imm + 1, 5);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save opcode */
        strcat(compressed, "01");
        free(new_imm);
    }
    else if (!strcmp(name_16, "addi"))
    {
        strncpy(rd, binary + 20, 5);/*get rd*/
        strncpy(immed, binary, 12);
        imm = sign_decimal(immed);/*get imm*/
        new_imm = decimal_to_binary(imm, 6);
        /* Save func3 */
        strcpy(compressed, "000");
        /* Save nzimm[5] */
        strncpy(tmp, new_imm, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save rd */
        strcat(compressed, rd);
        /* Save nzimm[4:0] */
        strncpy(tmp, new_imm + 1, 5);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save opcode */
        strcat(compressed, "01");
        free(new_imm);
    }
    else if (!strcmp(name_16, "slli"))
    {
        strncpy(rd, binary + 20, 5);/*get rd*/
        strcpy(compressed, "000");/*get pre*/
        strcat(compressed, "0");
        strncpy(tmp, binary + 7, 5);
        strcat(compressed, rd);/*add rd*/
        strcat(compressed, tmp);
        strcat(compressed, "10");/*add last*/
    }
    else if (!strcmp(name_16, "lw"))
    {
        /* Load rd */
        strncpy(rd, binary + 20, 5);
        /* Load rs1 */
        strncpy(rs1, binary + 12, 5);
        /* Load imm[11:0] */
        strncpy(immed, binary, 12);
        /* Transfer imm to 7 bits */
        imm = decimal(immed, 0, 11);
        new_imm = decimal_to_binary(imm, 7);
        /* Transfer rd to 3 bits */
        reg1_id = decimal(rd, 0, 4);
        reg1_id -= 8;
        bit3_rd = decimal_to_binary(reg1_id, 3);
        /* Transfer rs1 to 3 bits */
        reg2_id = decimal(rs1, 0, 4);
        reg2_id -= 8;
        bit3_rs = decimal_to_binary(reg2_id, 3);
        /* Generate compressed instruction */
        /* Save func3 */
        strcpy(compressed, "010");
        /* Save new_imm[5:3] */
        strncpy(tmp, new_imm + 1, 3);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save rs1 */
        strcat(compressed, bit3_rs);
        /* Save new_imm[2] */
        strncpy(tmp, new_imm + 4, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save new_imm[6] */
        strncpy(tmp, new_imm, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save rd */
        strcat(compressed, bit3_rd);
        /* Save opcode */
        strcat(compressed, "00");
        /* Free allocated memory */
        free(bit3_rd);
        free(bit3_rs);
        free(new_imm);
    }
    else if (!strcmp(name_16, "sw"))/*type sw*/
    {
        /* Load rd */
        strncpy(rd, binary + 12, 5);
        /* Load rs1 */
        strncpy(rs1, binary + 7, 5);
        /* Load imm */
        /* Load imm[11:5] */
        strncpy(immed, binary, 7);
        /* Load imm[4:0] */
        strncpy(tmp, binary + 20, 5);
        strcat(immed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Transfer imm to 7 bits */
        imm = decimal(immed, 0, 11);
        new_imm = decimal_to_binary(imm, 7);
        /* Transfer rd to 3 bits */
        reg1_id = decimal(rd, 0, 4);
        reg1_id -= 8;
        bit3_rd = decimal_to_binary(reg1_id, 3);
        /* Transfer rs1 to 3 bits */
        reg2_id = decimal(rs1, 0, 4);
        reg2_id -= 8;
        bit3_rs = decimal_to_binary(reg2_id, 3);
        /* Save func3 */
        strcpy(compressed, "110");
        /* Save new_imm[5:3] */
        strncpy(tmp, new_imm + 1, 3);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save rd */
        strcat(compressed, bit3_rd);
        /* Save new_imm[2] */
        strncpy(tmp, new_imm + 4, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save new_imm[6] */
        strncpy(tmp, new_imm, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save rs1 */
        strcat(compressed, bit3_rs);
        /* Save opcode */
        strcat(compressed, "00");
        /* Free allocated memory */
        free(bit3_rd);
        free(bit3_rs);
        free(new_imm);
    }
    else if (!strcmp(name_16, "and"))/*type and*/
    {
        /* Load rd */
        strncpy(rd, binary + 20, 5);
        /* Load rs2 */
        strncpy(rs2, binary + 7, 5);
        /* Transfer rd to 3 bits */
        reg1_id = decimal(rd, 0, 4);
        reg1_id -= 8;
        bit3_rd = decimal_to_binary(reg1_id, 3);
        /* Transfer rs2 to 3 bits */
        reg2_id = decimal(rs2, 0, 4);
        reg2_id -= 8;
        bit3_rs = decimal_to_binary(reg2_id, 3);
        /* Save func6 */
        strcpy(compressed, "100011");
        /* Save rd */
        strcat(compressed, bit3_rd);
        /* Save func2 */
        strcat(compressed, "11");
        /* Save rs2 */
        strcat(compressed, bit3_rs);
        /* Save opcode */
        strcat(compressed, "01");
        free(bit3_rd);
        free(bit3_rs);
    }
    else if (!strcmp(name_16, "or"))/*type or*/
    {
        /* Load rd */
        strncpy(rd, binary + 20, 5);
        /* Load rs2 */
        strncpy(rs2, binary + 7, 5);
        /* Transfer rd to 3 bits */
        reg1_id = decimal(rd, 0, 4);
        reg1_id -= 8;
        bit3_rd = decimal_to_binary(reg1_id, 3);
        /* Transfer rs2 to 3 bits */
        reg2_id = decimal(rs2, 0, 4);
        reg2_id -= 8;
        bit3_rs = decimal_to_binary(reg2_id, 3);
        /* Save func6 */
        strcpy(compressed, "100011");
        /* Save rd */
        strcat(compressed, bit3_rd);
        /* Save func2 */
        strcat(compressed, "10");
        /* Save rs2 */
        strcat(compressed, bit3_rs);
        /* Save opcode */
        strcat(compressed, "01");
        free(bit3_rd);
        free(bit3_rs);
    }
    else if (!strcmp(name_16, "xor"))/*type xor*/
    {
        /* Load rd */
        strncpy(rd, binary + 20, 5);
        /* Load rs2 */
        strncpy(rs2, binary + 7, 5);
        /* Transfer rd to 3 bits */
        reg1_id = decimal(rd, 0, 4);
        reg1_id -= 8;
        bit3_rd = decimal_to_binary(reg1_id, 3);
        /* Transfer rs2 to 3 bits */
        reg2_id = decimal(rs2, 0, 4);
        reg2_id -= 8;
        bit3_rs = decimal_to_binary(reg2_id, 3);
        /* Save func6 */
        strcpy(compressed, "100011");
        /* Save rd */
        strcat(compressed, bit3_rd);
        /* Save func2 */
        strcat(compressed, "01");
        /* Save rs2 */
        strcat(compressed, bit3_rs);
        /* Save opcode */
        strcat(compressed, "01");
        free(bit3_rd);
        free(bit3_rs);
    }
    else if (!strcmp(name_16, "sub"))/*type sub*/
    {
        /* Load rd */
        strncpy(rd, binary + 20, 5);
        /* Load rs2 */
        strncpy(rs2, binary + 7, 5);
        /* Transfer rd to 3 bits */
        reg1_id = decimal(rd, 0, 4);
        reg1_id -= 8;
        bit3_rd = decimal_to_binary(reg1_id, 3);
        /* Transfer rs2 to 3 bits */
        reg2_id = decimal(rs2, 0, 4);
        reg2_id -= 8;
        bit3_rs = decimal_to_binary(reg2_id, 3);
        /* Save func6 */
        strcpy(compressed, "100011");
        /* Save rd */
        strcat(compressed, bit3_rd);
        /* Save func2 */
        strcat(compressed, "00");
        /* Save rs2 */
        strcat(compressed, bit3_rs);
        /* Save opcode */
        strcat(compressed, "01");
        free(bit3_rd);
        free(bit3_rs);
    }
    else if (!strcmp(name_16, "beqz"))/*type beqz*/
    {
        /* Load src */
        strncpy(rs1, binary + 12, 5);
        /* Transfer src to 3 bits */
        reg2_id = decimal(rs1, 0, 4);
        reg2_id -= 8;
        bit3_rs = decimal_to_binary(reg2_id, 3);
        /* Load imm */
        /* Load imm[12] */
        strncpy(immed, binary, 1);
        /* Load imm[11] */
        strncpy(tmp, binary + 24, 1);
        strcat(immed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Load imm[10:5] */
        strncpy(tmp, binary + 1, 6);
        strcat(immed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Load imm[4:1] */
        strncpy(tmp, binary + 20, 4);
        strcat(immed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Add imm[0] */
        strcat(immed, "0");
        /* Transfer imm to 9 bits */
        imm = sign_decimal(immed);
        new_imm = decimal_to_binary(imm, 9);
        /* Save func3 */
        strcpy(compressed, "110");
        /* Save new_imm[8] */
        strncpy(tmp, new_imm, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save new_imm[4:3] */
        strncpy(tmp, new_imm + 4, 2);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save src */
        strcat(compressed, bit3_rs);
        /* Save new_imm[7:6] */
        strncpy(tmp, new_imm + 1, 2);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save new_imm[2:1] */
        strncpy(tmp, new_imm + 6, 2);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save new_imm[5] */
        strncpy(tmp, new_imm + 3, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save opcode */
        strcat(compressed, "01");
        /* Free the allocated memory */
        free(bit3_rs);
        free(new_imm);
    }
    else if (!strcmp(name_16, "bnez"))/*type bnez*/
    {
        /* Load src */
        strncpy(rs1, binary + 12, 5);
        /* Transfer src to 3 bits */
        reg2_id = decimal(rs1, 0, 4);
        reg2_id -= 8;
        bit3_rs = decimal_to_binary(reg2_id, 3);
        /* Load imm */
        /* Load imm[12] */
        strncpy(immed, binary, 1);
        /* Load imm[11] */
        strncpy(tmp, binary + 24, 1);
        strcat(immed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Load imm[10:5] */
        strncpy(tmp, binary + 1, 6);
        strcat(immed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Load imm[4:1] */
        strncpy(tmp, binary + 20, 4);
        strcat(immed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Add imm[0] */
        strcat(immed, "0");
        /* Transfer imm to 9 bits */
        imm = sign_decimal(immed);
        new_imm = decimal_to_binary(imm, 9);
        /* Save func3 */
        strcpy(compressed, "111");
        /* Save new_imm[8] */
        strncpy(tmp, new_imm, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save new_imm[4:3] */
        strncpy(tmp, new_imm + 4, 2);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save src */
        strcat(compressed, bit3_rs);
        /* Save new_imm[7:6] */
        strncpy(tmp, new_imm + 1, 2);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save new_imm[2:1] */
        strncpy(tmp, new_imm + 6, 2);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save new_imm[5] */
        strncpy(tmp, new_imm + 3, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save opcode */
        strcat(compressed, "01");
        /* Free the allocated memory */
        free(bit3_rs);
        free(new_imm);
    }
    else if (!strcmp(name_16, "srli"))
    {
        /* Load dest */
        strncpy(rd, binary + 20, 5);
        /* Transfer dest to 3 bits */
        reg1_id = decimal(rd, 0, 4);
        reg1_id -= 8;
        bit3_rd = decimal_to_binary(reg1_id, 3);
        /* Load shamt[4:0] */
        strncpy(tmp, binary + 7, 5);
        /* Save func3 */
        strcpy(compressed, "100");
        /* Save shamt[5] */
        strcat(compressed, "0");
        /* Save func2 */
        strcat(compressed, "00");
        /* Save dest */
        strcat(compressed, bit3_rd);
        /* Save shamt[4:0] */
        strcat(compressed, tmp);
        /* Save opcode */
        strcat(compressed, "01");
        /* Free the allocated memory */
        free(bit3_rd);
    }
    else if (!strcmp(name_16, "srai"))
    {
        /* Load dest */
        strncpy(rd, binary + 20, 5);
        /* Transfer dest to 3 bits */
        reg1_id = decimal(rd, 0, 4);
        reg1_id -= 8;
        bit3_rd = decimal_to_binary(reg1_id, 3);
        /* Load shamt[4:0] */
        strncpy(tmp, binary + 7, 5);
        /* Save func3 */
        strcpy(compressed, "100");
        /* Save shamt[5] */
        strcat(compressed, "0");
        /* Save func2 */
        strcat(compressed, "01");
        /* Save dest */
        strcat(compressed, bit3_rd);
        /* Save shamt[4:0] */
        strcat(compressed, tmp);
        /* Save opcode */
        strcat(compressed, "01");
        /* Free the allocated memory */
        free(bit3_rd);
    }
    else if (!strcmp(name_16, "andi"))
    {
        /* Load dest */
        strncpy(rd, binary + 12, 5);
        /* Transfer dest to 3 bits */
        reg1_id = decimal(rd, 0, 4);
        reg1_id -= 8;
        bit3_rd = decimal_to_binary(reg1_id, 3);
        /* Load imm[11:0] */
        strncpy(immed, binary, 12);
        /* Transfer imm to 6 bits */
        imm = sign_decimal(immed);
        new_imm = decimal_to_binary(imm, 6);
        /* Save func3 */
        strcpy(compressed, "100");
        /* Save new_imm[5] */
        strncpy(tmp, new_imm, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save func2 */
        strcat(compressed, "10");
        /* Save dest */
        strcat(compressed, bit3_rd);
        /* Save new_imm[4:0] */
        strncpy(tmp, new_imm + 1, 5);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save opcode */
        strcat(compressed, "01");
        /* Free the allocated memory */
        free(bit3_rd);
        free(new_imm);
    }
    else if (!strcmp(name_16, "j"))
    {
        /* Load imm[20] */
        strncpy(immed, binary, 1);
        /* Load imm[19:12] */
        strncpy(tmp, binary + 12, 8);
        strcat(immed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Load imm[11] */
        strncpy(tmp, binary + 11, 1);
        strcat(immed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Load imm[10:1] */
        strncpy(tmp, binary + 1, 10);
        strcat(immed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Add imm[0] */
        strcat(immed, "0");
        /* Transfer imm to 12 bits */
        imm = sign_decimal(immed);
        new_imm = decimal_to_binary(imm, 12);
        /* Save func3 */
        strcpy(compressed, "101");
        /* Save new_imm[11] */
        strncpy(tmp, new_imm, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save new_imm[4] */
        strncpy(tmp, new_imm + 7, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save new_imm[9:8] */
        strncpy(tmp, new_imm + 2, 2);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save new_imm[10] */
        strncpy(tmp, new_imm + 1, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save new_imm[6] */
        strncpy(tmp, new_imm + 5, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save new_imm[7] */
        strncpy(tmp, new_imm + 4, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save new_imm[3:1] */
        strncpy(tmp, new_imm + 8, 3);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save new_imm[5] */
        strncpy(tmp, new_imm + 6, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save opcode */
        strcat(compressed, "01");
        /* Free the allocated memory */
        free(new_imm);
    }
    else if (!strcmp(name_16, "jal"))
    {
        /* Load imm[20] */
        strncpy(immed, binary, 1);
        /* Load imm[19:12] */
        strncpy(tmp, binary + 12, 8);
        strcat(immed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Load imm[11] */
        strncpy(tmp, binary + 11, 1);
        strcat(immed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Load imm[10:1] */
        strncpy(tmp, binary + 1, 10);
        strcat(immed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Add imm[0] */
        strcat(immed, "0");
        /* Transfer imm to 12 bits */
        imm = sign_decimal(immed);
        new_imm = decimal_to_binary(imm, 12);
        /* Save func3 */
        strcpy(compressed, "001");
        /* Save new_imm[11] */
        strncpy(tmp, new_imm, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save new_imm[4] */
        strncpy(tmp, new_imm + 7, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save new_imm[9:8] */
        strncpy(tmp, new_imm + 2, 2);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save new_imm[10] */
        strncpy(tmp, new_imm + 1, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save new_imm[6] */
        strncpy(tmp, new_imm + 5, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save new_imm[7] */
        strncpy(tmp, new_imm + 4, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save new_imm[3:1] */
        strncpy(tmp, new_imm + 8, 3);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save new_imm[5] */
        strncpy(tmp, new_imm + 6, 1);
        strcat(compressed, tmp);
        memset(tmp, 0, sizeof(tmp));
        /* Save opcode */
        strcat(compressed, "01");
        /* Free the allocated memory */
        free(new_imm);
    }
    return compressed;
}
