CC = gcc
CFLAGS = -Wpedantic -Wall -Wextra -Werror -std=c89

main: main.c doubll.o
	${CC} ${CFLAGS} main.c doubll.o -o main
	tar -czvf hw2.tar doubll.c staticlib.sh dynamiclib.sh

doubll.o: doubll.c doubll.h
	${CC} ${CFLAGS} -c doubll.c -o doubll.o
	# ${CC} ${CFLAGS} -DCHECK_LIST -c doubll.c -o doubll.o

