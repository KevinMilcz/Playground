#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "ascii_to_asm.h"
/* http://ref.x86asm.net/coder32.html */

static const char * strings[] =
{
"INC	%EAX /*40 @ */",
"INC 	%ECX /*41 A */",
"INC 	%EDX /*42 B */",
"INC 	%EBX /*43 C */",
"INC 	%ESP /*44 D */",
"INC 	%EBP /*45 E */",
"INC 	%ESI /*46 F */",
"INC 	%EDI /*47 G */",
"DEC 	%EAX /*48 H */",
"DEC 	%ECX /*49 I */",
"DEC 	%EDX /*4A J */",
"DEC 	%EBX /*4B K */",
"DEC 	%ESP /*4C L */",
"DEC 	%EBP /*4D M */",
"DEC 	%ESI /*4E N */",
"DEC 	%EDI /*4F O */",
"PUSH 	%EAX /*50 P */",
"PUSH 	%ECX /*51 Q */",
"PUSH 	%EDX /*52 R */",
"PUSH 	%EBX /*53 S */",
"PUSH 	%ESP /*54 T */",
"PUSH 	%EBP /*55 U */",
"PUSH 	%ESI /*56 V */",
"PUSH 	%EDI /*57 W */",
"POP 	%EAX /*58 X */",
"POP 	%ECX /*59 Y */",
"POP 	%EDX /*5A Z */",
"POP 	%EBX /*5B [ */",
"POP 	%ESP /*5C \\ */", //Disabled, changing the stack that much makes it hard to recover
"POP 	%EBP /*5D ] */",
"POP 	%ESI /*5E ^ */",
"POP 	%EDI /*5F _ */",
};
static const size_t numStrings = sizeof(strings) / sizeof(strings[0]);
#define FIRST_INDEX 0x40
#define LAST_INDEX  0x5F

#define PUSH_IX		0x50
#define	PUSH_IX_END	0x57
#define POP_IX		0x58
#define POP_IX_END	0x5F

#define DEC_ESP		0x4C
#define INC_ESP		0x44
#define POP_ESP		0x5c

static int stackDelta = 0;
static int maxStack = 0;
static int minStack = 0;

static int stackCalc(char c)
{
	int stackChange = 0;
	c = toupper(c);
	if(c == POP_ESP){
		stackChange = 0; //instruction disabled
	}else if(c >= PUSH_IX && c <= PUSH_IX_END) {
		stackChange = -4;
	} else if(c >= POP_IX && c <= POP_IX_END) {
		stackChange = 4;
	} else if(c == DEC_ESP ) {
		stackChange = -1;
	} else if( c == INC_ESP) {
		stackChange = 1;
	}
	stackDelta += stackChange;

	if(stackDelta > maxStack) {
		maxStack = stackDelta;
	}

	if(stackDelta < minStack) {
		minStack = stackDelta;
	}
	return stackChange;
}

static inline int table_idx(char c)
{
	if(isspace(c)) {
		return (c + 0x3F) -FIRST_INDEX; //Turns it into an underscore
	}

	c = toupper(c);

	if(c == POP_ESP){
		fprintf(stderr, "Can't translate character %c to asm! \n", c);
		return 0;
	}

	if(c >= FIRST_INDEX && c <= LAST_INDEX) {
		return c - FIRST_INDEX;
	} else {
		fprintf(stderr, "Can't translate character %c to asm! \n", c);
		return 0;
	}
}

static inline const char* getStr(char c)
{
	stackCalc(c);
	return strings[table_idx(c)];
}

void convert_string(const char* s)
{
	size_t bufSize = 0;
	char* buffer;
	FILE * stream = open_memstream(&buffer, &bufSize);

	/* Write out the "string" instructions to a temp buffer */
	while(*s)
	{
		fprintf(stream, "%s\n", getStr(*s));
		s++;
	}
	fflush (stream);

	/* We're going to try to produce runnable code by:
	 * 1) Calculate the total stack space change, adjust in other direction
	 * 2) Disable popping into ESP, so our initial stack is preserved.
	 * 3) Save all registers before running user code, restore them after
	 */

	/* Setup the header */
	printf(".code32\n");
	printf(".globl your_function\n");
	printf("your_function:\n");

	printf("ENTER \t$4, $0\n");
	//Save the registers to a fixed location, ignore eax (return register, value doesn't matter) */
	printf("MOV \t$saved_regs, %%EAX\n");
	printf("MOV \t%%EBX, 0x00(%%EAX)\n");
	printf("MOV \t%%ECX, 0x04(%%EAX)\n");
	printf("MOV \t%%EDX, 0x08(%%EAX)\n");
	printf("MOV \t%%EDI, 0x0C(%%EAX)\n");
	printf("MOV \t%%ESI, 0x10(%%EAX)\n");
	printf("MOV \t%%ESP, 0x14(%%EAX)\n");
	printf("MOV \t%%EBP, 0x18(%%EAX)\n");

	//They wanted to pop a bunch of stuff, so make room for them to do it without stepping on
	//Other stuff
	if(maxStack > 0) {
		printf("SUB \t$%d, %%ESP\n", maxStack);
	}

	printf(".globl your_string\n");
	printf("your_string:\n");

	//Copy the instrucstions that make up the string out to stdout
	printf("%s", buffer);

	/* Null terminator. The de-ref is fine because we disallow popping the stack into ESP
	 * They could have pushed enough to run out of stack, but that will crash before this	 */
	printf("ADD \t%%ah, (%%ESP)\n");

	//Restore the registers
	printf("MOV \t$saved_regs, %%EAX\n");
	printf("MOV \t0x00(%%EAX),%%EBX\n");
	printf("MOV \t0x04(%%EAX),%%ECX\n");
	printf("MOV \t0x08(%%EAX),%%EDX\n");
	printf("MOV \t0x0C(%%EAX),%%EDI\n");
	printf("MOV \t0x10(%%EAX),%%ESI\n");
	printf("MOV \t0x14(%%EAX),%%ESP\n");
	printf("MOV \t0x18(%%EAX),%%EBP\n");
	printf("XOR \t%%EAX, %%EAX\n");
	printf("LEAVE\n");
	printf("RET\n");
	printf(".section .data\n");
	printf("saved_regs:\n.skip 0x40\n");

	if(buffer)
	{
		fclose(stream);
		free(buffer);
	}
}

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		fprintf(stderr, "Usage: <string to convert>\n"
						"Converts a string to GCC compile-able 32bit assembly\n"
						"Output is to stdout\n");
		return -1;
	}

	convert_string(argv[1]);

	return 0;
}


