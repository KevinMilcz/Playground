#include <stdio.h>
#include <time.h>

#include "ascii_to_asm.h"

extern char your_string;
extern void your_function(void);

char __attribute__((weak)) your_string = '\0';

void __attribute__((weak)) your_function(void)
{
	fprintf(stderr, "Didn't want to get to this function\n");
	fflush(stderr);
	return;
}

int main(int argc, char** argv)
{
	char *s = &your_string;
	while(*s)
	{
		printf("(0x%hX)[%c] ", *s, *s);
		s++;
	}
	printf("\n");
	fflush(stdout);

	your_function();

	//Some system calls to see if stuff is still sane...
	clock_t my_time = clock();
	int secs = my_time * CLOCKS_PER_SEC;
	printf("If this text is printing, your code probably works! Time = %d\n", secs);

	return 0;
}
