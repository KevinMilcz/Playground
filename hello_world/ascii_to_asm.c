#include <stdio.h>
#include <string.h>
#include <ctype.h>

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
"POP 	%ESP /*5C \\ */",
"POP 	%EBP /*5D ] */",
"POP 	%ESI /*5E ^ */",
"POP 	%EDI /*5F _ */",
};
static const size_t numStrings = sizeof(strings) / sizeof(strings[0]);
#define FIRST_INDEX 0x40
#define LAST_INDEX  0x5F

static inline int table_idx(char c)
{
	if(isspace(c)) {
		return (c + 0x3F) -FIRST_INDEX; //Turns it into an underscore
	}

	c = toupper(c);

	if(c >= FIRST_INDEX && c <= LAST_INDEX) {
		return c - FIRST_INDEX;
	} else {
		fprintf(stderr, "Can't translate character %c to asm! \n", c);
		return 0;
	}
}

static inline const char* getStr(char c)
{
	return strings[table_idx(c)];
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

	char *s = argv[1];
	printf(".code32\n");
	printf(".globl your_asm\n");
	printf("your_asm:\n");

	while(*s)
	{
		printf("%s\n", getStr(*s));
		s++;
	}
	printf(".byte 0\n\n");

	return 0;
}
