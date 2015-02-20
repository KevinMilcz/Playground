#include <stdio.h>
#include <stdint.h>

/* define a type for a function that takes a pointer to a void pointer as
 * as an argument, and returns a void pointer */
typedef void* (*func)(void**);

/* pointer to an array of function pointers with type func */
func (*a)[10];

/* Input is a pointer to a function pointer, presumably with some accessible memory
 * after it (an array perhaps?). This function simply copies that function pointer, 
 * to the next spot in the array, and prints out the index offset from the global a */
void* findNext(void** arg)
{
	void**next = (void**)((uintptr_t)arg + sizeof(arg));
	*next = *arg;
	printf("index: %d\n", (int)( ((uintptr_t)next - (uintptr_t)*a) / sizeof(arg) -1) );
	return *next;	
}

int main(int argc, char**argv)
{
	size_t i;	
	/* Make some space for a to point to, and assign it*/
	void* b[sizeof(*a)/sizeof(**a)];
	a = (void*)buf;
	
	/* Populate the first function pointer */
	(*a)[0] = findNext;
	
	/* Loop through a, calling each function pointer, passing a pointer to the function
         * pointer as an argument. Since we only populated the first index,
	 * pray that someone else filled in the next one before we call it... */
	for(i = 0; i < sizeof(*a)/sizeof(**a) -1; i++)
	{	
		(*a)[i]( (void*)&(*a)[i] );
	}
	
	/* Just to shut off unused param warning*/
	return argc ? !argc : (argv != NULL);
}
