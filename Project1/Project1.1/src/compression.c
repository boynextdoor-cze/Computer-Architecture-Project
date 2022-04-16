#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "compression.h"
#include "utils.h"

/* Compress serves as the main function of compression */
char *Compress(char read[])
{
    /* ans is the final output of an input */
    char *ans;
    char type;
    /* name_xx refers to the name of each instruction in 32 and 16 bits */
    char *name_32, *name_16;
    char n32[10], n16[10];
    /* Firstly, determine the type of instruction */
    type = type_determine(read);
    /* Secondly, determine the name of 32-bit instruction */
    name_32 = name_32_determine(type, read);
    /* If the 32-bit instruction is not compressible, then return the origin binary code */
    if (!strcmp(name_32, "False"))
    {
        /* Free the allocated memory */
        free(name_32);
        /* Copy the original binary code to ans */
        ans= (char *)malloc(sizeof(char) * 40);
        /*allocate memory*/
        strcpy(ans, read);
        return ans;
    }
    /* printf("name_32: %s\n", name_32); */
    strcpy(n32, name_32);
    /* For each potentially compressible instruction, check if it is really compressible and get a 16-bit name */
    name_16 = name_16_determine(n32, read);
    /* If the instruction is not compressible, then return the origin binary code */
    if (!strcmp(name_16, "False"))
    {
        /* Free the allocated memory */
        free(name_32);
        free(name_16);
        /* Copy the original binary code to ans */
        ans= (char *)malloc(sizeof(char) * 40);
        /*allocate memory*/
        strcpy(ans, read);
        return ans;
    }
    /* printf("name_16: %s\n", name_16); */
    strcpy(n16, name_16);
    /* If the instruction is compressible, then compress it */
    ans = compress(n16, read);
    /* printf("%s\n", ans); */
    /* Free the allocated memory */
    free(name_16);
    free(name_32);
    /* Return the compressed instruction */
    return ans;
}