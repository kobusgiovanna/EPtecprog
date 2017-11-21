#include "instr.h"

#define PILMAX 99

typedef struct {
    int topo;
    OPERANDO val[PILMAX+1];
} Pilha;
static void Erro(char *msg);
static void Fatal(char *msg, int cod);
Pilha *cria_pilha();
void destroi_pilha(Pilha *p);
void empilha(Pilha *p, OPERANDO op);
OPERANDO desempilha(Pilha *p);
void imprime(Pilha *p, int n);
void empilhaint(Pilha *p, int n);