#ifndef INSTR_H
#define INSTR_H

/* Códigos das instruções */
typedef enum {
  PUSH,
  POP,
  DUP,
  ADD,
  SUB,
  MUL,
  DIV,
  JMP,
  JIT,
  JIF,
  CALL,
  RET,
  STS,
  RCS,
  EQ,
  GT,
  GE,
  LT,
  LE,
  NE,
  STO,
  RCL,
  END,
  PRN,
  SAVE,
  REST,
  STL,
  RCE,
  ALC,
  FRE,
  ATR,
  ENTRY,
  LEAVE
} OpCode;

//Tipos dos operandos
typedef enum {
  NUM,
  ACAO,
  VAR,
  CELULA
} Tipo;

////Tipos de terreno
typedef enum {
  ESTRADA,
  MONTANHA,
  RIO,
  BASE
} Terreno;

//Celula
typedef struct{
  int terreno;
    // 0 = estrada
    // 1 = montanha
    // 2 = rio
    // 3 = base
  //se for base
  int base;
  short int cristais;
  int ocupado;
} Celula;

//Operando
typedef struct {
  Tipo t;
  union{
    int n;
    int ac;
    int v;
    Celula cel;
  } val;
} OPERANDO;

//Instrução
typedef struct {
  OpCode instr;
  OPERANDO op;
} INSTR;

#endif
