CC := cc
LD := ld
NASM := nasm
CFLAGS := -ffreestanding -nostdlib -fno-builtin -I../../libc/include
LDFLAGS := -m elf_x86_64 -nostdlib -z text -z max-page-size=0x1000 -T linker.ld
ASMFLAGS := -f elf64

SRC := main.c
ASM := crt0.asm
OBJ := $(SRC:.c=.o) $(ASM:.asm=.o)

all: bin/init

../../libc/lib/libc.a:
	$(MAKE) -C ../../libc

bin/init: $(OBJ) ../../libc/lib/libc.a
	mkdir -p bin
	$(LD) $(LDFLAGS) -o bin/init $(OBJ) ../../libc/lib/libc.a

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(NASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf $(OBJ) bin
