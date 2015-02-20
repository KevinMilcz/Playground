#include <stdio.h>
#include <stdint.h>

/* define a type for a function that takes an array of void pointers as
 * as an argument, and returns a void pointer */
typedef void* (*func)(void* arg[]);

/* pointer to an array of function pointers with type func */
func (*a)[10];

/* Input is an array of void pointers, which are actually function pointers but we can't
 * really specify a function as taking an argument with a funtion pointer with the same signature.
 * (it goes recursive)
 * This function simply copies the function pointer in index 0 to index 1, and 
 * prints out the index offset from the global a */
void* findNext(void* arg[])
{
	arg[1] = arg[0];
	printf("index: %d\n", (int)( ((uintptr_t)&arg[1] - (uintptr_t)*a) / sizeof(*arg) -1) );
	return arg[1];	
}

int main(int argc, char**argv)
{
	size_t i;	
	/* Make some space for a to point to, and assign it*/
	void* buf[sizeof(*a)/sizeof(**a)];
	a = (void*)buf;
	
	/* Populate the first function pointer */
	(*a)[0] = findNext;
	
	/* Loop through a, calling each function pointer, passing a pointer to the function
         * pointer as an argument. Since we only populated the first index,
	 * pray that someone else filled in the next one before we call it... */
	for(i = 0; i < sizeof(*a)/sizeof(**a) -1; i++)
	{	
		(*a)[i]( (void**)&(*a)[i] );
	}
	
	/* Just to shut off unused param warning*/
	return argc ? !argc : (argv != NULL);
}
