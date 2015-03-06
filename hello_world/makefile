EXES := hello_world ascii_to_asm

.PHONY: all clean

all: $(EXES)

clean:
	rm -f $(EXES)

hello_world: hello_world.S
	$(CC) -m64 $< -o $@

ascii_to_asm: ascii_to_asm.c
	$(CC) -Wall -Wextra $< -o $@
