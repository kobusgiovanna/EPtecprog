#include "pilha.c"

#define MAXMEM 100

typedef struct {
  Pilha pil;
  Pilha exec;
  OPERANDO Mem[MAXMEM];
  INSTR *prog;
  int ip;
  int rbp;
  int cristais;
  int x, y;
  int baseX, baseY;
  int vida;
    union{
        int cristais[6];
        int bases[6];
        int amigos[6];
        int inimigos[6];
    }proxPosition;
} Maquina;

Maquina *cria_maquina(INSTR *p);

void destroi_maquina(Maquina *m);

void exec_maquina(Maquina *m, int n);
