#ifndef TRANSLATOR_H
#define TRANSLATOR_H

/* Run the translator */
int translate(const char *in, const char *out);

/* Update the offsets with respect to jump and branch instructions */
static void fix_offset(char **bins,int lines);

/* Print outcome to the output file */
static void print(char **bins, int lines, FILE *output);

/* Update the offset of CB_1 type */
static void CB_1_convert(char **bins, int line, char mcode[]);

/* Update the offset of CJ type */
static void CJ_convert(char **bins, int line, char mcode[]);

/* Update the offset of B type */
static void B_convert(char **bins, int line, char mcode[]);

/* Calculate new offset */
static int New_offset(char **bin, int line, int imm);

#endif