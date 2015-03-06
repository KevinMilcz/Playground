EXES := hello_world ascii_to_asm
CFLAGS = -Wall -Wextra -std=gnu99 -g

.PHONY: all clean

all: $(EXES)

clean:
	rm -f $(EXES)

hello_world: hello_world.S
	$(CC) -m64 $< -o $@

ascii_to_asm: ascii_to_asm.c
	$(CC) $(CFLAGS) $< -o $@
