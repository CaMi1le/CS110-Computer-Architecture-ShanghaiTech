CC=gcc
CFLAGS= -O0 -std=gnu99

all: sum
sum: sum.c
	$(CC) $(CFLAGS) -o sum sum.c
	
opt: sum.c
	$(CC) -std=gnu99 -O2 -o opt sum.c

sseTest.s : sseTest.c
	$(CC) -S sseTest.c

clean:
	rm -rf sum
