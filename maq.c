#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "maq.h"
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

int tempo=0;
//vetor que armazena cada maquina
Maquina *a[110];
Celula arena[200][200];

//declaro os vetores de movimento
int movx[6]={0,1,1,0,-1,-1};
int movy[6]={-1,-1,0,1,1,0};


//registro dos ponteiros das maquinas no vetor a[] de robos
void registro_maquina(Maquina *m){
    int i = 0;
    while(a[i] != NULL)
        i++;
    a[i] = m;
}


//cria uma maquina m, estabelece uma coordenada nao ocupada para a mesma
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
    m->vida = 100;
    m->cristais = 0;
    m->ip = 0;
    m->prog = p;
    return m;
}

void destroi_maquina(Maquina *m) {
    free(m);
}

//faz a base do time z no par (x,y)
void fazbase(int x,int y,int z){
    while(arena[x][y].ocupado == 1 || arena[x][y].base != 0){
        srand(time(NULL));
        int coordX = rand()%200;
        int coordY = rand()%200;
        x = coordX;
        y = coordY;
    }
    arena[x][y].terreno = 3;
    arena[x][y].base = z;
}

//adiciona o x-ésimo exercito
//vincula uma base para cada membro de exercito
//exercito = grupo de robos/tropas
void InsereExercito(int x, int tropas, INSTR *p){
    Maquina *maq;
    srand(time(NULL));
    int coordX = rand()%200;
    int coordY = rand()%200;
    fazbase(coordX, coordY, x);
    for(int j = 0; j < tropas; j++){
        maq = cria_maquina(p);
        maq -> baseX = coordX;
        maq -> baseY = coordY;
        registro_maquina(maq);
    }
}

//remove o exercito x da base (i,j)
void RemoveExercito(int x){
    for(int i = 0; i < 110; i++){
        if(arena[a[i]->baseX][a[i] ->baseY].base == x){
            destroi_maquina(a[i]);
            a[i] = NULL;
        }
    }
}

//movimenta cada maquina m (n instruções)
void Atualiza(int n){
    tempo++;
    for(int i = 0; i < 110; i++)
        if(a[i] != NULL)
            exec_maquina(a[i], 50);
        else break;
    
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

void constroi(){
    //construcao randomica, seed com tempo pra arena variar
    srand(time(NULL));

    //podemos setar as probabilidades de cada tipo de terreno, em %;
    double probESTRADA=40;
    double probMONTANHA=40;
    double probRIO=20;

    for(int i=0;i<200;i++){
        for(int j=0;j<200;j++){
            //nao há robos nem bases na inicialização
            arena[i][j].ocupado = 0;
            arena[i][j].base = 0;
            //colocaremos as bases depois
            int terreno;
            int tipo=rand()%100;

            if(tipo < probESTRADA)
                terreno=0;
            else if(tipo < probESTRADA+probMONTANHA)
                terreno=1;
            else 
                terreno=2;

            tipo=rand()%100;

            //definiremos o numero de cristais em cada posicao
            if(tipo<95)
                arena[i][j].cristais=0;
            else if(tipo<99)
                arena[i][j].cristais=1;
            else 
                arena[i][j].cristais=2;

            arena[i][j].terreno=terreno;
        }
    }
}
//system calls
//movimentar os robos
// 0=N,1=NE,2=SE,3=S,4=SW,5=NW
int isvalid(int x,int y){
    if(x>=0 && y>=0 && x<200 && y<200)return 1;
    return 0;
}

//move a maquina até coordenada (x,y)
void Move(Maquina *soldier, int dir, Celula arena[200][200], int n){
    if(isvalid(soldier->x + n*movx[dir],soldier->y + n*movy[dir])){
        if((arena[soldier->x + n*movx[dir]][soldier->y + n*movy[dir]]).ocupado == 0){
            arena[soldier->x][soldier->y].ocupado = 0;
            soldier->x = soldier->x + n*movx[dir];
            soldier->y = soldier->y + n*movx[dir];
            arena[soldier->x][soldier->y].ocupado = 1;
            vizinhanca(soldier, soldier->x, soldier->y);
        }else {printf("%s", "A Celula ja estava ocupada");}
    }else {printf("%s", "Movimento fora dos limites da arena.");}
}
//recolhe cristal
void Retrieve(Maquina *soldier, int dir, Celula arena[200][200]){
    if(isvalid(soldier->x + movx[dir],soldier->y + movy[dir])){
        if((arena[soldier->x + movx[dir]][soldier->y + movy[dir]]).cristais > 0){
            (arena[soldier->x + movx[dir]][soldier->y + movy[dir]]).cristais-=1;
            soldier->cristais+=1;
            vizinhanca(soldier, soldier->x, soldier->y);
        }else{printf("%s", "Nao ha cristal nessa posicao");}
    }else{printf("%s", "Posicao invalida");}
}
//deposita cristal
void Put(Maquina *soldier, int dir, Celula arena[200][200]){
    if(isvalid(soldier->x + movx[dir],soldier->y + movy[dir])){
        if(soldier->cristais > 0){
            (arena[soldier->x + movx[dir]][soldier->y + movy[dir]]).cristais ++;
            soldier->cristais--;
            vizinhanca(soldier, soldier->x, soldier->y);
        }else{printf("%s", "Seu soldado nao tem cristais");}
    }else{printf("%s", "Posicao invalida");}
}

//verifica os arredores da celula (x,y)
void vizinhanca(Maquina *m ,int i, int j){
    for(int k = 0; k < 6; k++){
        int x = i+movx[k];
        int y = j+movy[k];
        //verifica se ha cristais
        if(arena[x][y].cristais > 0){
            m->proxPosition.cristais[k] = arena[x][y].cristais;
        }else
            m->proxPosition.cristais[k] = 0;
        //verifica se há bases (0 -> sem base)
        if(arena[x][y].base > 0){
            m->proxPosition.bases[k] = arena[x][y].base;
        }else
            m->proxPosition.bases[k] = 0;
        //verifica se há maquinas
        if(arena[x][y].ocupado > 0){
            if(arena[x][y].base == arena[i][j].base)
                m->proxPosition.amigos[k] = 1;
            else
                m->proxPosition.inimigos[k] = 1;
        }else{
            m->proxPosition.amigos[k] = 0;
            m->proxPosition.inimigos[k] = 0;
        }
        
    }
}

//insere n cristais na celula (i,j)
void PutCristal(int n, int i, int j){
    arena[i][j].cristais += n;
}

