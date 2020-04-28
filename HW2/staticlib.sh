gcc -Wpedantic -Wall -Wextra -Werror -std=c89 -c test.c -o test.o
gcc -Wpedantic -Wall -Wextra -Werror -std=c89 -c doubll.c -o doubll.o
ar -r liblist.a doubll.o
gcc -o staticlist test.o -L . -Bstatic -llist
