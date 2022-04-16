/*  Project 1.1: RISC-V instructions to RISC-V compressed instructions in C89.
    The following is the starter code provided for you. To finish the task, you
    should define and implement your own functions in translator.c, compression.c,
    utils.c and their header files.
    Please read the problem description before you start.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "src/compression.h"
#include "src/utils.h"

#include "translator.h"

/*check if file can be correctly opened */
static int open_files(FILE **input, FILE **output, const char *input_name, const char *output_name)
{
    *input = fopen(input_name, "r");
    if (!*input)
    { /* open input file failed */
        printf("Error: unable to open input file: %s\n", input_name);
        return -1;
    }

    *output = fopen(output_name, "w");
    if (!*output)
    { /* open output file failed */
        printf("Error: unable to open output file: %s\n", output_name);
        fclose(*input);
        return -1;
    }
    return 0; /* no problem opening files */
}

static int close_files(FILE **input, FILE **output)
{
    fclose(*input);
    fclose(*output); /* close the files at the end */
    return 0;
}

static void print_usage_and_exit()
{
    printf("Usage:\n");
    printf("Run program with translator <input file> <output file>\n"); /* print the correct usage of the program */
    exit(0);
}

/* Calculate new offset */
static int New_offset(char **bins, int line, int imm)
{
    int dest, instr_number, new_offset = 0;
    int i;
    /* Calculate how many instructions are jumped over */
    instr_number = imm / 4;
    /* Calculate the destination line of instruction */
    dest = line + instr_number;
    if (dest < line)
    {
        for (i = dest; i < line; ++i)
            /* If compressed instruction */
            if (strlen(bins[i]) == 16)
                new_offset += 2;
            /* If non-compressed instruction */
            else if (strlen(bins[i]) == 32)
                new_offset += 4;
        new_offset *= -1;
    }
    else if (dest > line)/*another situation*/
    {
        for (i = line; i < dest; ++i)
            /* If compressed instruction */
            if (strlen(bins[i]) == 16)
                new_offset += 2;
            /* If non-compressed instruction */
            else if (strlen(bins[i]) == 32)
                new_offset += 4;
    }
    return new_offset;
}

/* Update the offset of CB_1 type */
static void CB_1_convert(char **bins, int line, char mcode[])
{
    /* imm[] is the concatenated imm string, tmpx[] stores small slices */
    char imm[10] = {0}, tmp1[3] = {0}, tmp2[3] = {0}, tmp3[3] = {0}, tmp4[3] = {0}, tmp5[3] = {0};
    int immediate, new_offset;
    /* new_imm[] is the updated binary code with respect to new offset*/
    char *new_imm;
    /* Copy imm[8] */
    strncpy(tmp1, mcode + 3, 1);
    /* Copy imm[4:3] */
    strncpy(tmp2, mcode + 4, 2);
    /* Copy imm[7:6] */
    strncpy(tmp3, mcode + 9, 2);
    /* Copy imm[2:1] */
    strncpy(tmp4, mcode + 11, 2);
    /* Copy imm[5] */
    strncpy(tmp5, mcode + 13, 1);
    /* Concat all slices in correct order */
    strcpy(imm, tmp1); /* Got imm[8] */
    strcat(imm, tmp3); /* Got imm[8:6] */
    strcat(imm, tmp5); /* Got imm[8:5] */
    strcat(imm, tmp2); /* Got imm[8:3] */
    strcat(imm, tmp4); /* Got imm[8:1] */
    strcat(imm, "0"); /* Add imm[0] */
    /* Calculate immediate */
    immediate = sign_decimal(imm);
    /* Calculate new offset */
    new_offset = New_offset(bins, line, immediate);
    /* Calculate new binary code with respect to new immediate */
    new_imm = decimal_to_binary(new_offset, 9);
    /* Update offset in translated instructions */
    bins[line][3] = new_imm[0];  /* Update offset[8] */
    bins[line][4] = new_imm[4];  /* Update offset[4] */
    bins[line][5] = new_imm[5];  /* Update offset[3] */
    bins[line][9] = new_imm[1];  /* Update offset[7] */
    bins[line][10] = new_imm[2]; /* Update offset[6] */
    bins[line][11] = new_imm[6]; /* Update offset[2] */
    bins[line][12] = new_imm[7]; /* Update offset[1] */
    bins[line][13] = new_imm[3]; /* Update offset[5] */
    free(new_imm);
}

/* Update the offset of CJ type */
static void CJ_convert(char **bins, int line, char mcode[])
{
    /* imm[] is the concatenated imm string, tmpx[] stores small slices */
    char imm[15] = {0}, tmp1[3] = {0}, tmp2[3] = {0}, tmp3[3] = {0}, tmp4[3] = {0}, tmp5[3] = {0}, tmp6[3] = {0}, tmp7[5] = {0}, tmp8[3] = {0};
    int immediate, new_offset;
    /* new_imm[] is the updated binary code with respect to new offset*/
    char *new_imm;
    strncpy(tmp1, mcode + 3, 1);  /* Copy imm[11] */
    strncpy(tmp2, mcode + 4, 1);  /* Copy imm[4] */
    strncpy(tmp3, mcode + 5, 2);  /* Copy imm[9:8] */
    strncpy(tmp4, mcode + 7, 1);  /* Copy imm[10] */
    strncpy(tmp5, mcode + 8, 1);  /* Copy imm[6] */
    strncpy(tmp6, mcode + 9, 1);  /* Copy imm[7] */
    strncpy(tmp7, mcode + 10, 3); /* Copy imm[3:1] */
    strncpy(tmp8, mcode + 13, 1); /* Copy imm[5] */
    /* Concat all slices in correct order */
    strcpy(imm, tmp1); /* Got imm[11] */
    strcat(imm, tmp4); /* Got imm[11:10] */
    strcat(imm, tmp3); /* Got imm[11:8] */
    strcat(imm, tmp6); /* Got imm[11:7] */
    strcat(imm, tmp5); /* Got imm[11:6] */
    strcat(imm, tmp8); /* Got imm[11:5] */
    strcat(imm, tmp2); /* Got imm[11:4] */
    strcat(imm, tmp7); /* Got imm[11:1] */
    strcat(imm, "0");  /* Add imm[0] */
    /* Calculate immediate */
    immediate = sign_decimal(imm);
    /* Calculate new offset */
    new_offset = New_offset(bins, line, immediate);
    /* Calculate new binary code with respect to new immediate */
    new_imm = decimal_to_binary(new_offset, 12);
    /* Update offset in translated instructions */
    bins[line][3] = new_imm[0];   /* Update offset[11] */
    bins[line][4] = new_imm[7];   /* Update offset[4] */
    bins[line][5] = new_imm[2];   /* Update offset[9] */
    bins[line][6] = new_imm[3];   /* Update offset[8] */
    bins[line][7] = new_imm[1];   /* Update offset[10] */
    bins[line][8] = new_imm[5];   /* Update offset[6] */
    bins[line][9] = new_imm[4];   /* Update offset[7] */
    bins[line][10] = new_imm[8];  /* Update offset[3] */
    bins[line][11] = new_imm[9];  /* Update offset[2] */
    bins[line][12] = new_imm[10]; /* Update offset[1] */
    bins[line][13] = new_imm[6];  /* Update offset[5] */
    free(new_imm);
}

/* Update the offset of B type */
static void B_convert(char **bins, int line, char mcode[])
{
    /* imm[] is the concatenated imm string, tmpx[] stores small slices */
    char imm[15] = {0}, tmp1[10] = {0}, tmp2[10] = {0}, tmp3[10] = {0}, tmp4[10] = {0};
    int immediate, new_offset;
    /* new_imm[] is the updated binary code with respect to new offset*/
    char *new_imm;
    strncpy(tmp1, mcode, 1);      /* Copy imm[12] */
    strncpy(tmp2, mcode + 1, 6);  /* Copy imm[10:5] */
    strncpy(tmp3, mcode + 20, 4); /* Copy imm[4:1] */
    strncpy(tmp4, mcode + 24, 1); /* Copy imm[11] */
    /* Concat all slices in correct order */
    strcpy(imm, tmp1); /* Got imm[12] */
    strcat(imm, tmp4); /* Got imm[12:11] */
    strcat(imm, tmp2); /* Got imm[12:5] */
    strcat(imm, tmp3); /* Got imm[12:1] */
    strcat(imm, "0");  /* Add imm[0] */
    /* Calculate immediate */
    immediate = sign_decimal(imm);
    /* Calculate new offset */
    new_offset = New_offset(bins, line, immediate);
    /* Calculate new binary code with respect to new immediate */
    new_imm = decimal_to_binary(new_offset, 13);
    /* Update offset in translated instructions */
    bins[line][0] = new_imm[0];   /* Update offset[12] */
    bins[line][1] = new_imm[2];   /* Update offset[10] */
    bins[line][2] = new_imm[3];   /* Update offset[9] */
    bins[line][3] = new_imm[4];   /* Update offset[8] */
    bins[line][4] = new_imm[5];   /* Update offset[7] */
    bins[line][5] = new_imm[6];   /* Update offset[6] */
    bins[line][6] = new_imm[7];   /* Update offset[5] */
    bins[line][20] = new_imm[8];  /* Update offset[4] */
    bins[line][21] = new_imm[9];  /* Update offset[3] */
    bins[line][22] = new_imm[10]; /* Update offset[2] */
    bins[line][23] = new_imm[11]; /* Update offset[1] */
    bins[line][24] = new_imm[1];  /* Update offset[11] */
    free(new_imm);
}

/* Update the offsets with respect to jump and branch instructions */
static void fix_offset(char **bins, int lines)
{
    int i, len;
    char tmp[40] = {0}, opcode[10] = {0}, func3[5] = {0};
    for (i = 0; i < lines; ++i)
    {
        /* Initialize strings each time */
        memset(tmp, 0, sizeof(tmp));
        memset(opcode, 0, sizeof(opcode));
        memset(func3, 0, sizeof(func3));
        /* Use tmp to copy the ith line of binary code */
        strcpy(tmp, bins[i]);
        len = strlen(tmp);
        /* Check the length of this line */
        if (len == 16) /* If length=16, the last 2 digits are opcode, and the first 3 digits are func3 */
        {
            /* Extract opcode and func3*/
            strncpy(opcode, tmp + 14, 2);
            strncpy(func3, tmp, 3);
            /*
            printf("%s\n", opcode);
            printf("%s\n", func3);
            */
            /* Determine format */
            /* If it is CB_1, then update format */
            if (!strcmp(opcode, "01") && (!strcmp(func3, "110") || !strcmp(func3, "111")))
                CB_1_convert(bins, i, tmp);
            /* If it is CJ, then update format */
            if (!strcmp(opcode, "01") && (!strcmp(func3, "101") || !strcmp(func3, "001")))
                CJ_convert(bins, i, tmp);
        }
        else if (len == 32) /* If length=32, the last 7 digits are opcode, and the 18th-20th digits are func3 */
        {
            /* Extract opcode and func3 */
            strncpy(opcode, tmp + 25, 7);
            strncpy(func3, tmp + 17, 3);
            /* Determine format */
            /* If it is B-type, then update offset */
            if (!strcmp(opcode, "1100011") && (!strcmp(func3, "000") || !strcmp(func3, "001") || !strcmp(func3, "100") || !strcmp(func3, "101") || !strcmp(func3, "110") || !strcmp(func3, "111")))
                B_convert(bins, i, tmp);
        }
    }
}

/* Print outcome to the output file */
static void print(char **bins, int lines, FILE *output)
{
    int i;
    for (i = 0; i < lines; ++i)
        fprintf(output, "%s\n", bins[i]);
}

/*Run the translator */
int translate(const char *in, const char *out)
{
    FILE *input, *output;
    /* read[] stores the presently read line */
    char read[40];
    /* bins is a 2D array, storing the temporary outcome of all lines of binary code */
    char **bins;
    /* write stores the presently written outcome */
    char *write;
    int err = 0, lines = 0;
    int i;
    if (in)
    { /* correct input file name */
        if (open_files(&input, &output, in, out) != 0)
            exit(1);
        /* Memory allocation */
        bins = (char **)malloc(60 * sizeof(char *));
        for (i = 0; i < 60; ++i)
            bins[i] = (char *)malloc(40 * sizeof(char));
        /* Read a line from input file and store in 'read'. */
        while (fscanf(input, "%s", read) != EOF)
        {

            /* Invoke Compress() in compression.c to process the binary code, and the outcome is stored in 'write'. */
            write = Compress(read);
            strcpy(bins[lines++], write);
            /* If compression failed, increase err. */
            if (write == NULL)
                err++;
            free(write);
        }
        /* Adjust offsets in jump and branch instructions */
        fix_offset(bins, lines);
        /* Write output binary code to output file. */
        print(bins, lines, output);
        /* Free memory */
        for (i = 0; i < 60; ++i)
            free(bins[i]);
        free(bins);
        /* Close file */
        close_files(&input, &output);
    }
    return err;
}

/* main func */
int main(int argc, char **argv)
{
    char *input_fname, *output_fname;
    int err;

    if (argc != 3) /* need correct arguments */
        print_usage_and_exit();

    input_fname = argv[1];
    output_fname = argv[2];

    err = translate(input_fname, output_fname); /* main translation process */
    if (err)
        printf("One or more errors encountered during translation operation.\n"); /* something wrong */
    else
        printf("Translation process completed successfully.\n"); /* correctly output */

    return 0;
}