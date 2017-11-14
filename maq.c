#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "arena.c"
// #define DEBUG
#ifdef DEBUG
#  define D(X) X
char *CODES[] = {
    "PUSH",
    "POP",
    "DUP",
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "JMP",
    "JIT",
    "JIF",
    "CALL",
    "RET",
    "STS",
    "RCS",
    "EQ",
    "GT",
    "GE",
    "LT",
    "LE",
    "NE",
    "STO",
    "RCL",
    "END",
    "PRN",
    "STL",
    "RCE",
    "ALC",
    "FRE",
    "ATR"
};
#else
#  define D(X)
#endif

static void Erro(char *msg) {
    fprintf(stderr, "%s\n", msg);
}

static void Fatal(char *msg, int cod) {
    Erro(msg);
    exit(cod);
}


Maquina *cria_maquina(INSTR *p) {
    srand(time(NULL));
    int coordX = rand()%200;
    int coordY = rand()%200;
    Maquina *m = (Maquina*)malloc(sizeof(Maquina));
    if (!m) Fatal("Memória insuficiente",4);
    m -> x = coordX;
    m -> y = coordY;
        do{
            int coordX = rand()%200;
            int coordY = rand()%200;
            m -> x = coordX;
            m -> y = coordY;
        }while(arena[coordX][coordY].ocupado == 1);
    arena[coordX][coordY].ocupado = 1;
    m->cristais = 0;
    m->ip = 0;
    m->prog = p;
    return m;
}

void destroi_maquina(Maquina *m) {
    free(m);
}

/* Alguns macros para facilitar a leitura do código */
#define ip (m->ip)
#define pil (&m->pil)
#define exec (&m->exec)
#define prg (m->prog)
#define rbp (m->rbp)
#define topo (exec->topo) // Representa o topo da pilha de execução

void exec_maquina(Maquina *m, int n) {
    int i;
    for (i = 0; i < n; i++) {
        OpCode   opc = prg[ip].instr;
        OPERANDO arg = prg[ip].op;
        
        D(printf("%3d: %-4.4s %d\n     ", ip, CODES[opc], arg));
        
        switch (opc) {
                OPERANDO tmp;
                OPERANDO op1;
                OPERANDO op2;
                OPERANDO res;
            case PUSH:
                empilha(pil, arg);
                break;
            case POP:
                desempilha(pil);
                break;
            case DUP:
                tmp = desempilha(pil);
                empilha(pil, tmp);
                empilha(pil, tmp);
                break;
            case ADD:
                op1 = desempilha(pil);
                op2 = desempilha(pil);
                
                if (op1.t == NUM && op2.t == NUM) {
                    res.t = NUM;
                    res.val.n = op1.val.n  + op2.val.n;
                    empilha(pil, res);
                }
                break;
            case SUB:
                op1 = desempilha(pil);
                op2 = desempilha(pil);
                
                if (op1.t == NUM && op2.t == NUM) {
                    res.t = NUM;
                    res.val.n = op1.val.n  - op2.val.n;
                    empilha(pil, res);
                }
                break;
            case MUL:
                op1 = desempilha(pil);
                op2 = desempilha(pil);
                
                if (op1.t == NUM && op2.t == NUM) {
                    res.t = NUM;
                    res.val.n = op1.val.n  * op2.val.n;
                    empilha(pil, res);
                }
                break;
            case DIV:
                op1 = desempilha(pil);
                op2 = desempilha(pil);
                
                if (op1.t == NUM && op2.t == NUM) {
                    res.t = NUM;
                    res.val.n = op1.val.n  / op2.val.n;
                    empilha(pil, res);
                }
                break;
            case JMP:
                ip = arg.val.n;
                continue;
            case JIT:
                op1 = desempilha(pil);
                if (op1.val.n != 0) {
                    ip = arg.val.n;
                    continue;
                }
                break;
            case JIF:
                op1 = desempilha(pil);
                if (op1.val.n == 0) {
                    ip = arg.val.n;
                    continue;
                }
                break;
            case CALL:
                tmp.val.n = ip;
                empilha(exec, tmp);
                tmp.val.n = rbp;
                empilha(exec, tmp);
                ip = arg.val.n;
                rbp = topo;
                continue;
            case RET:
                op1 = desempilha(exec);
                op2 = desempilha(exec);
                rbp = op1.val.n;
                ip = op2.val.n;
                break;
            case EQ:
                op1 = desempilha(pil);
                op2 = desempilha(pil);
                if (op1.val.n == op2.val.n){
                    tmp.val.n = 1;
                    empilha(pil, tmp);
                }
                else{
                    tmp.val.n = 0;
                    empilha(pil, tmp);
                }
                break;
            case GT:
                op1 = desempilha(pil);
                op2 = desempilha(pil);
                if (op1.val.n < op2.val.n){
                    tmp.val.n = 1;
                    empilha(pil, tmp);
                }
                else{
                    tmp.val.n = 0;
                    empilha(pil, tmp);
                }
                break;
            case GE:
                op1 = desempilha(pil);
                op2 = desempilha(pil);
                if (op1.val.n <= op2.val.n){
                    tmp.val.n = 1;
                    empilha(pil, tmp);
                }
                else{
                    tmp.val.n = 0;
                    empilha(pil, tmp);
                }
                break;
            case LT:
                op1 = desempilha(pil);
                op2 = desempilha(pil);
                if (op1.val.n > op2.val.n){
                    tmp.val.n = 1;
                    empilha(pil, tmp);
                }
                else{
                    tmp.val.n = 0;
                    empilha(pil, tmp);
                }
                break;
            case LE:
                op1 = desempilha(pil);
                op2 = desempilha(pil);
                if (op1.val.n >= op2.val.n){
                    tmp.val.n = 1;
                    empilha(pil, tmp);
                }
                else{
                    tmp.val.n = 0;
                    empilha(pil, tmp);
                }
                break;
            case NE:
                op1 = desempilha(pil);
                op2 = desempilha(pil);
                if (op1.val.n != op2.val.n){
                    tmp.val.n = 1;
                    empilha(pil, tmp);
                }
                else{
                    tmp.val.n = 0;
                    empilha(pil, tmp);
                }
                break;
            case STO:
                m->Mem[arg.val.n] = desempilha(pil);
                break;
            case RCL:
                empilha(pil,m->Mem[arg.val.n]);
                break;
            case END:
                return;
            case PRN:
                op1 = desempilha(pil);
                printf("%d\n", op1.val.n);
                break;
            case STL:
                (exec)->val[arg.val.n + rbp - 1] = desempilha(pil); //
                break;
            case RCE:
                empilha(pil, (exec)->val[arg.val.n + rbp - 1]);
                break;
            case ALC:
                topo = topo + arg.val.n;
                break;
            case FRE:
                for(i = 0; i < arg.val.n; i++) {
                    desempilha(exec);
                }
                break;
            case ATR:
                tmp = desempilha(pil);
                switch(arg.val.n) {
                    case 0:
                        empilhaint(pil, tmp.val.cel.terreno);
                        break;
                    case 1:
                        empilhaint(pil, tmp.val.cel.cristais);
                        break;
                    case 2:
                        empilhaint(pil, tmp.val.cel.ocupado);
                        break;
                    case 3:
                        empilhaint(pil, tmp.val.cel.base);
                        break;
                }
        }
        
        D(printf("Topo: %d, RBP: %d\n", topo, rbp ));
        D(imprime(pil,10));
        D(puts("\n"));
        D(imprime(exec,20));
        D(puts("\n\n"));
        
        ip++;
    }
}
