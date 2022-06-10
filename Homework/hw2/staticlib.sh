gcc -Wpedantic -Wall -Wextra -Werror -std=c89 -c ringbuffer.c -o ringbuffer.o
gcc -Wpedantic -Wall -Wextra -Werror -std=c89 -c test.c -o test.o
ar -r libringbuffer.a ringbuffer.o
ld test.o libringbuffer.a -lc -o staticringbuffer