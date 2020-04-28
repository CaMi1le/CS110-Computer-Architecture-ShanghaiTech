
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define PRINT_SIZE(X) printf("Size of "#X": %lu\n", sizeof(X))

int main()
{
	PRINT_SIZE(char);
	PRINT_SIZE(short);
	PRINT_SIZE(short int);
	PRINT_SIZE(long int);
	PRINT_SIZE(unsigned int);
	PRINT_SIZE(void *);
	PRINT_SIZE(size_t);
	PRINT_SIZE(float);
	PRINT_SIZE(int8_t);
	PRINT_SIZE(int16_t);
	PRINT_SIZE(int32_t);
	PRINT_SIZE(int64_t);
	PRINT_SIZE(time_t);
	PRINT_SIZE(clock_t);
	PRINT_SIZE(struct tm);
	PRINT_SIZE(time_t);
	PRINT_SIZE(NULL);

    return 0;
}
