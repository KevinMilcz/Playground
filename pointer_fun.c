#include <stdio.h>
#include <stdint.h>

void* (*(*a)[__LINE__])(void*);

void* findNext(void* arg)
{
	void**next = (void*)((uintptr_t)arg + sizeof(arg));
	*next = *(void**)arg;
	printf("index: %lu\n", ((uintptr_t)next - (uintptr_t)*a) / sizeof(arg) -1 );
	return *next;	
}

int main(int argc, char**argv)
{
	size_t i;	
	void* b[sizeof(*a)/sizeof(**a)];
	a = (void*)b;
	(*a)[0] = findNext;
	for(i = 0; i < sizeof(*a)/sizeof(**a) -1; i++)
	{	
		(*a)[i]( &(*a)[i] );
	}
	
	/* Just to shut off unused param warning*/
	return argc ? !argc : (argv != NULL);
}
