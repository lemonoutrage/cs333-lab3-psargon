CC = gcc
AS = nasm

DEFINES =
CFLAGS = -m32 -g3 -O0 -Wall -Werror -std=c11 -pedantic  -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement -Wno-return-local-addr -Wunsafe-loop-optimizations -Wuninitialized -Wextra
AFLAGS = -f elf32 -g3 -F dwarf
LDFLAGS = -m32 -lm -no-pie -lz

CPROG1 = psargon.c
OPROG1 = psargon.o
PROG1 = psargon

targets = $(PROG1)

all: $(targets)


$(PROG1): $(OPROG1)
	$(CC) $(LDFLAGS) -o $(PROG1) $(OPROG1)

$(OPROG1):
	$(CC) $(CFLAGS) -c $(CPROG1)


.PHONY: clean


clean:
	rm -f *.o *.lst $(targets) *~ \#*
ASSIGNMENT = 2
TAR_FILE = grif29_labtwo.tar.gz
tar:
	rm -f $(TAR_FILE)
	tar czf $(TAR_FILE) makefile arvik.c
backup:
	git add .
	git commit -m "Backup on $(shell date)"
	git push