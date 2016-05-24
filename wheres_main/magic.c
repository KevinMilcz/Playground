#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

int magic(int magic_dust, char** particles) {
	puts("hello world");
	for(int i = 1; i < magic_dust; ++i)
		puts(particles[i]);
	return 0;
}

struct i
{
	uint8_t o[2];
	uint64_t a;
} __attribute__((packed));

struct i main[2]  __attribute__((section (".text"))) = {{0x48, 0xb8, (uintptr_t)magic}, {0xFF, 0xE0, 0xC3}};

