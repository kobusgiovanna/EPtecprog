all: instr pilha maq motor os
instr: instr.h
pilha: pilha.h pilha.c
	gcc -c pilha.c
maq: maq.h maq.c
	gcc -c maq.c
motor: motor.c
	gcc -c motor.c
os:
	gcc pilha.o maq.o motor.o -o a.out
clean:
	rm -rf *.o

