all: instr pilha maq motor
instr: instr.h
pilha: pilha.h pilha.c
	gcc pilha.c
maq: maq.h maq.c
	gcc maq.c
motor: motor.c
	gcc motor.c
clean:
	rm -rf *.o

