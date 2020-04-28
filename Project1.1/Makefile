CC = gcc
CFLAGS = -g -std=c89 -Wall -Wpedantic -Wextra
ASSEMBLER_FILES = src/tables.c src/utils.c src/translate_utils.c src/translate.c

all: assembler

assembler: clean
	$(CC) $(CFLAGS) -o assembler assembler.c $(ASSEMBLER_FILES)

submission:
	mkdir -p .temp/framework \
	&& cp -r ./src assembler.c assembler.h Makefile run-valgrind .temp/framework/ \
	&& cd .temp/ && tar -czf ../framework.tar framework/

clean:
	rm -rf *.o assembler test-assembler core framework.tar .temp/
