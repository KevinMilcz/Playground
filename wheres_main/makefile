IGNORE_DUMB_FLAGS := -Wno-missing-braces
CFLAGS := -std=gnu11 -O3 -fno-strict-aliasing -Wall -Wextra -Wpedantic $(IGNORE_DUMB_FLAGS)

FAIRY_TAIL := fairy_tail
MAGIC_TRICK := magic_trick

.PHONY: all
all: $(FAIRY_TAIL) $(MAGIC_TRICK)

$(FAIRY_TAIL): marry_had_a_lamb.c
	gcc $(CFLAGS) $< -o $@

$(MAGIC_TRICK): magic.c
	gcc $(CFLAGS) $< -o $@

test: all
	X="$$(./fairy_tail)"; ./magic_trick $${X}

clean:
	rm -f $(FAIRY_TAIL) $(MAGIC_TRICK)
