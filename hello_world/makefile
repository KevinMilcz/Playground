CFLAGS = -Wall -Wextra -std=gnu99 -m32 -g

TEST_DIR := test_code

ATA := ascii_to_asm
ATA_TEST := ata_test
TEST_ASM := $(TEST_DIR)/test_asm.s
EXES := hello_world $(ATA) $(ATA_TEST)

.PHONY: all clean test

all: $(EXES)

clean:
	rm -f $(EXES) *.o $(TEST_DIR)/*

$(TEST_DIR):
	mkdir -p $@

hello_world: hello_world.S
	$(CC) -m64 $< -o $@

$(ATA): $(ATA).c 
	$(CC) $(CFLAGS) $< -o $@

$(TEST_ASM): $(ATA) | $(TEST_DIR)
	./$(ATA) "This is a long string that breaks things      " > $@

test: $(TEST_ASM) $(ATA) ata_test.c
	$(CC) $(CFLAGS) -c $(TEST_ASM) -o $(TEST_ASM).o
	$(CC) $(CFLAGS) $(TEST_ASM).o $(ATA_TEST).c -o $(ATA_TEST)
	
	