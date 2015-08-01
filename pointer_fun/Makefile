CFLAGS = -Wall -Wextra -Wpedantic -Werror -pedantic-errors -O3

OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))
EXES := pf pf_clean

.PHONY: all clean
all: $(EXES)

clean:
	rm -f $(EXES) $(OBJECTS)

pf: pointer_fun.o
	$(CC) $(CFLAGS) $< -o $@

pf_clean: pf_clean.o
	$(CC) $(CFLAGS) $< -o $@


