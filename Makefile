all: instr pilha maq motor os compila.tab.o compila
instr: instr.h
pilha: pilha.h pilha.c
	gcc -c pilha.c
maq: maq.h maq.c
	gcc -c maq.c
motor: motor.c
	gcc -c motor.c 
os:
	gcc pilha.o maq.o motor.o -o a.out

compila: compila.tab.o lex.yy.o symrec.o acertos.o maq.o pilha.o motor.o
	gcc -o $@ $^ -lm -lfl

compila.tab.o: compila.y
	bison -d compila.y
	gcc -c compila.tab.c

lex.yy.o: compila.l
	flex compila.l
	gcc -c lex.yy.c

clean:
	rm -f *.o lex.yy.c compila.tab.c compila.tab.h *~ compila

