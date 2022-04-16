#ifndef UTILS_H
#define UTILS_H

/* Calculate 2 to the power of n */
int power(int base, int n);

/*change the binary code into decimal*/
int decimal(char binary[] ,int first,int last);

/* Convert a twos-compliment into a decimal number */
int sign_decimal(char binary[]);

/* Convert a decimal number to a binary string */
char *decimal_to_binary(int num,int size);

/*determine the RV32 format*/
char type_determine(char binary[]);
/*determine the possible 32-name*/
char* name_32_determine(char type, char binary[]);

/*determine the 16-name*/
char* name_16_determine(char name_32[], char binary[]);
/*return the compressed value*/
char* compress(char name_16[],char binary[]);

#endif