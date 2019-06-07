#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stddef.h>

char main[4096 * 2] = "Marry had a little lamb, its fleece was white as snow\n";

int had_a_little_lamb(void) {
	mprotect((void*)((uintptr_t)main & ~(4096ULL-1)), 4096, 3);
	for(char* s = &main[sizeof(main)-1]; *s;) putchar(*s--);
	return 0;
}

int marry() {
	return had_a_little_lamb();
}

void __attribute__((constructor)) fairy_tail(void) {		
	for(char* s = main, *d = &main[sizeof(main)-1]; *s;) *d-- = *s++;
	main[0] = (char)0xE9;
	*((int32_t*) &main[1]) = (int32_t)((ptrdiff_t)marry - (ptrdiff_t)&main[5]);
	mprotect((void*)((uintptr_t)main & ~(4096ULL-1)), 4096, 5);
}
