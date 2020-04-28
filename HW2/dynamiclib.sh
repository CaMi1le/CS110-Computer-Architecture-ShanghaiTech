gcc -Wpedantic -Wall -Wextra -Werror -std=c89 -c test.c -o test.o
gcc -fPIC -shared -Wpedantic -Wall -Wextra -Werror -std=c89 -c doubll.c -o liblist.so 
gcc -o dynamiclist test.o -L . -Bdynamic -llist