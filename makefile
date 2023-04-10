CFLAGS = -ansi -Wall -pedantic # Flags



all: assembler

assembler: main.o functions.o preassembler.o ht.o first_pass.o second_pass.o
	gcc main.o functions.o preassembler.o ht.o first_pass.o second_pass.o $(CFLAGS) -o assembler

main.o: main.c first_pass.h second_pass.h functions.h preassembler.h ht.h
	gcc -c main.c $(CFLAGS) -o $@

functions.o: functions.c functions.h
	gcc -c functions.c $(CFLAGS) -o $@

preassembler.o: preassembler.c preassembler.h
	gcc -c preassembler.c $(CFLAGS) -o $@

ht.o: ht.c ht.h
	gcc -c ht.c $(CFLAGS) -o $@

first_pass.o: first_pass.c first_pass.h ht.h
	gcc -c first_pass.c $(CFLAGS) -o $@

second_pass.o: second_pass.c second_pass.h ht.h
	gcc -c second_pass.c $(CFLAGS) -o $@

clean:
	rm -f *.o program

