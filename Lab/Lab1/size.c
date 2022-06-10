#include <stdio.h>
#include <stdint.h>
#include <time.h>
#define uint unsigned int
#define PRINT(x) (printf("Size of "#x": %d\n", (uint)sizeof(x)))
int main()
{
    PRINT(char);
    PRINT(short);
    PRINT(short int);
    PRINT(int);
    PRINT(long int);
    PRINT(unsigned int);
    PRINT(void *);
    PRINT(size_t);
    PRINT(float);
    PRINT(double);
    PRINT(int8_t);
    PRINT(int16_t);
    PRINT(int32_t);
    PRINT(int64_t);
    PRINT(time_t);
    PRINT(clock_t);
    PRINT(struct tm);
    PRINT(NULL);
    return 0;
}
/*
                          32-bit     64-bit
Size of char:               1           1
Size of short:              2           2
Size of short int:          2           2
Size of int:                4           4
Size of long int:           4           8
Size of unsigned int:       4           4
Size of void *:             4           8
Size of size_t:             4           8
Size of float:              4           4
Size of double:             8           8
Size of int8_t:             1           1
Size of int16_t:            2           2
Size of int32_t:            4           4
Size of int64_t:            8           8
Size of time_t:             4           8
Size of clock_t:            4           8
Size of struct tm:          44          56
Size of NULL:               4           8
*/
