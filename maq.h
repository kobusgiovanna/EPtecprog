#ifndef MAQ_H
#define MAQ_H


#include "pilha.h"

#define MAXMEM 100
#define MAXFRM 30

FILE * display;

typedef struct {
  Pilha pil;
  Pilha exec;
  OPERANDO Mem[MAXMEM];
  INSTR *prog;
  int id;
  int ip;
  int ib;
  int rbp;
  int bp[MAXFRM];
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

Maquina *cria_maquina(INSTR *p, int x, int y);

void destroi_maquina(Maquina *m);

void exec_maquina(Maquina *m, int n);

int new_frame(Maquina *m, int pos);

int del_frame(Maquina *m);

FILE * create_display();

#endif
