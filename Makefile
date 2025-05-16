CC = gcc
AS = nasm

DEFINES =
CFLAGS = -g3 -O0 -Wall -Werror -std=c11 -pedantic  -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement -Wno-return-local-addr -Wunsafe-loop-optimizations -Wuninitialized -Wextra -I/u/rchaney/argon2/include
AFLAGS = -f elf32 -g3 -F dwarf
LDFLAGS = -L/u/rchaney/argon2/lib/x86_64-linux-gnu -lm -no-pie -lz -lpthread /u/rchaney/argon2/lib/x86_64-linux-gnu/libargon2.so

CPROG1 = psargon.c
OPROG1 = psargon.o
PROG1 = psargon

targets = $(PROG1)

all: $(targets)


$(PROG1): $(OPROG1)
	$(CC) $(LDFLAGS) -o $(PROG1) $(OPROG1) $(LDFLAGS)

$(OPROG1): $(CPROG1)
	$(CC) $(CFLAGS) -c $(CPROG1)


.PHONY: clean


clean:
	rm -f *.o *.lst $(targets) *~ \#*

TAR_FILE = grif29_labthree.tar.gz
tar:
	rm -f $(TAR_FILE)
	tar czf $(TAR_FILE) Makefile psargon.c
backup:
	git add .
	git commit -m "Backup on $(shell date)"
	git push