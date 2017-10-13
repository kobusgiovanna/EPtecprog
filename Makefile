motor: maq.o pilha.o motor.o
	gcc -o motor maq.o pilha.o motor.o 

maq.o: maq.c
	gcc -c maq.c

pilha.o: pilha.c
	gcc -c pilha.c

motor.o: motor.c
	gcc -c motor.c

clean:
	rm -f motor *.o