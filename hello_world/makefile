OUT := hello_world

.PHONY: all

all: hello_world

hello_world: hello_world.S
	$(CC) -m64 $< -o $@
