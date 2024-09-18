CC := cc
LD := ld
CFLAGS := -ffreestanding -nostdinc -nostdlib -fno-builtin
LDFLAGS := -m elf_x86_64 -nostdlib -z text -z max-page-size=0x1000 -T linker.ld

SRC := main.c
OBJ := $(SRC:.c=.o)

all: test

test: $(OBJ)
	mkdir bin
	$(LD) $(LDFLAGS) -o bin/test $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ) bin
