
# Adapt this makefile to macos
SYSNAME := $(shell uname -s)
ifeq ($(SYSNAME), Linux)
	CC=gcc
endif
ifeq ($(SYSNAME), Darwin)
	CC=gcc-9
endif

CFLAGS=-Wpedantic -Wall -Werror -Wextra -std=c89 -g
SOURCE_FILES=shell.c parse.c

all: shell

shell: shell.c parse.c parse.h
	${CC} ${CFLAGS} ${SOURCE_FILES} -o shell

.PHONY: clean submission

test: shell
	./shell testcases/test${ID}.in

cmp: shell
	./shell testcases/test${ID}.in > testcases/test${ID}.out \
	&& bash testcases/test${ID}.in > testcases/test${ID}_std.out \
	&& cmp testcases/test${ID}_std.out testcases/test${ID}.out

clean:
	rm -f shell

memcheck: shell
	valgrind -v --tool=memcheck --leak-check=full --track-origins=yes ./shell testcases/test${ID}.in

submission:
	tar czvf project4.tar .git
