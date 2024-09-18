# This file is part of the Sphynx OS
# It is released under the MIT license -- see LICENSE
# Written by: Kevin Alavik.

CC := cc
AR := ar
CFLAGS := -ffreestanding -nostdlib -fno-builtin -Iinclude
ARFLAGS := rcs

SRC := $(wildcard src/*.c)
OBJ := $(SRC:src/%.c=src/%.o)
LIB := lib/libc.a

all: $(LIB)

$(LIB): $(OBJ)
	mkdir -p lib
	$(AR) $(ARFLAGS) $(LIB) $(OBJ)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf src/*.o $(LIB)
		