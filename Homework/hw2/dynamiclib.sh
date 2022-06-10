gcc -Wpedantic -Wall -Wextra -Werror -std=c89 -c test.c -o test.o
gcc -Wpedantic -Wall -Wextra -Werror -std=c89 -fPIC -shared -c ringbuffer.c -o libringbuffer.so
gcc -o dynamicringbuffer test.o -L . -lringbuffer
