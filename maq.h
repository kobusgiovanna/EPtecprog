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
<<<<<<< HEAD
  int ataque;
    union{
        int cristais[6];
        int bases[6];
        int amigos[6];
        int inimigos[6];
    }proxPosition;
=======
  //informacoes sobre a vizinhanca
  int vizi_cristais[6];
  int vizi_bases[6];
  int vizi_amigos[6];
  int vizi_inimigos[6];
>>>>>>> a5aa2e2ecaf77debb16dd244c59c20d5f8c12d14
} Maquina;

Maquina *cria_maquina(INSTR *p);

void destroi_maquina(Maquina *m);

void exec_maquina(Maquina *m, int n);
