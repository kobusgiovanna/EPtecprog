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
  int ataque;
  int vida;
  //informacoes sobre a vizinhanca
  int vizi_cristais[6];
  int vizi_bases[6];
  int vizi_amigos[6];
  int vizi_inimigos[6];
} Maquina;

Maquina *cria_maquina(INSTR *p);

void destroi_maquina(Maquina *m);

void exec_maquina(Maquina *m, int n);
