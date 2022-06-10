CXX = g++
CXX_FLAGS = -Wpedantic -Wall -Wextra -Werror -std=c++14
INC_FILES = ringbuffer.hpp ringbuffer.ipp
SRC_FILES = main.cpp

all: ${INC_FILES} ${SRC_FILES}
	${CXX} ${CXX_FLAGS} -o main ${SRC_FILES}

memcheck: main
	valgrind --tool=memcheck --leak-check=full ./main

submit:
	tar -cvf hw7.tar ringbuffer.ipp

clean:
	rm -f main

.PHONY: clean