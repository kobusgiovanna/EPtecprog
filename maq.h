#ifndef MAQ_H
#define MAQ_H


#include "pilha.h"

#define MAXMEM 100

FILE * display;

typedef struct {
  Pilha pil;
  Pilha exec;
  OPERANDO Mem[MAXMEM];
  INSTR *prog;
  int id;
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

FILE * create_display();

#endif