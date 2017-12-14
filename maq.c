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
    "ATR",
    "ENTRY",
    "LEAVE"
};
#else
#  define D(X)
#endif

int tempo=0;
//vetor que armazena cada maquina
Maquina *a[1000];
//vetor que armazena os pares (x,y) das bases
int bases[1000][2];
Celula arena[15][15];

// Determina a direção Y
int movy(int dir){
    int y[6] = {0, -1, -1, 0, 1, 1};
    return y[dir];
}

// Determina a direção X
int movx(int oy, int dir){
    int x[6];
    if (oy % 2){
        x[0] = -1;
        x[1] = 0;
        x[2] = 1;
        x[3] = 1;
        x[4] = 1;
        x[5] = 0;
    }
    else{
        x[0] = -1;
        x[1] = -1;
        x[2] = 0;
        x[3] = 1;
        x[4] = 0;
        x[5] = -1;
    }

    return x[dir];
}

FILE * display;

FILE * create_display(){
    FILE * _display;
    _display = popen("python3 apres", "w");
    return _display;
}

// Atualiza ou cria uma base
// dir: prefixo do diretório
// id : sufixo do diretório
// x  : coordenada horizontal
// y  : coordenada vertical
void update_base(char * dir, int id, int x, int y){
    fprintf(display, "base %s%d.png %d %d\n", dir, id, x, y);
    fflush(display);
}

// Atualiza ou cria um cristal
// n: quantidade de cristais
// x: coordenada horizontal
// y: coordenada vertical
void update_cristal(int n, int x, int y){
    fprintf(display, "cristal %d %d %d\n", n, x, y);
    fflush(display);
}

// Atualiza a posição de um robô
// i : quantidade de cristais
// ox: coordenada horizontal origem
// oy: coordenada vertical origem
// ix: coordenada horizontal destino
// iy: coordenada vertical destino
void update_robot(int i, int ox, int oy, int ix, int iy){
    fprintf(display, "%d %d %d %d %d\n", i, ox, oy, ix, iy);
    fflush(display);
}

// Cria um robô com imagem dada pelos argumentos
void create_robot(char * dir, int id){
    fprintf(display, "rob %s%d.png\n", dir, id);
    fflush(display);
}

//insere n cristais na celula (i,j)
void PutCristal(int n, int i, int j){
    arena[i][j].cristais += n;
    update_cristal(arena[i][j].cristais + n, i, j);
}


//registro dos ponteiros das maquinas no vetor a[] de robos
void registro_maquina(Maquina *m){
    int i = 0;
    while(a[i] != NULL)
        i++;
    a[i] = m;
    m->id = i;
}


//cria uma maquina m, estabelece uma coordenada nao ocupada para a mesma
Maquina *cria_maquina(INSTR *p,int x,int y) {
    Maquina *m = (Maquina*)malloc(sizeof(Maquina));
    if (!m) Fatal("Memória insuficiente",4);
    m -> x = x;
    m -> y = y;
    m->ataque = 30;
    m->vida = 100;
    m->cristais = 0;
    m->ip = 0;
    m->prog = p;
    arena[x][y].ocupado = 1;
    return m;
}

void destroi_maquina(Maquina *m) {
    update_robot(m->id, m->x, m->y, -1, -1);
    free(m);
}

//faz a base do time z no par (x,y)
int fazbase(int x,int y,int z){
    if(arena[x][y].ocupado || arena[x][y].base != 0 || arena[x][y].cristais)
        return 0;
    
    for(int i = 0; i < 6; i++)
        if(arena[x + movx(y, i)][y + movy(i)].ocupado)
            return 0;
    
    bases[z][0] = x;
    bases[z][1] = y;
    arena[x][y].terreno = 3;
    arena[x][y].base = z;
    arena[x][y].ocupado = 1;
    update_base("BASE_", z, x, y);
    return 1;
}

//adiciona o x-ésimo exercito
//vincula uma base para cada exercito
//cria seis tropas para cada exercito
void InsereExercito(int x, INSTR *p){

    srand(time(NULL));
    int coordX = 1 + rand()%13;
    int coordY = 1 + rand()%13;
    while(!fazbase(coordX, coordY, x)){
        coordX = 1 + rand()%13;
        coordY = 1 + rand()%13;
    }
    printf("Base em %d %d \n", coordX, coordY);
    for(int j = 0; j < 6; j++){
        Maquina *maq;
        maq = cria_maquina(p, coordX + movx(coordY, j), coordY + movy(j));
        maq -> baseX = bases[x][0];
        maq -> baseY = bases[x][1];
        registro_maquina(maq);
        create_robot("GILEAD_", x);
        update_robot(maq->id, -1, -1, maq->x, maq->y);
    }

}

//remove o exercito x
void RemoveExercito(int x){
    for(int i = 0; i < 1000; i++){
        if(arena[a[i]->baseX][a[i] ->baseY].base == x){
            destroi_maquina(a[i]);
            a[i] = NULL;
        }
    }
}

//movimenta cada maquina m (n instruções)
void Atualiza(int n){
    tempo++;
    for(int i = 0; i < 1000; i++)
        if(a[i] != NULL)
            exec_maquina(a[i], 50);
        else break;

}

int new_frame(Maquina *m, int n) {
    int ibc = m->ib;
    if (ibc < MAXFRM-1) {
        m->bp[++m->ib] = n+ibc;
        return m->ib;
    }
    return -1;
}

int del_frame(Maquina *m) {
    if (m->ib > 0) return --m->ib;
    return -1;
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
        
            case ENTRY:
                new_frame(m, arg.val.n);
                break;
                
            case LEAVE:
                del_frame(m);
                break;

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

    for(int i=0;i<15;i++){
        for(int j=0;j<15;j++){
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
            if(tipo<85)
                arena[i][j].cristais=0;
            else if(tipo<95){
                arena[i][j].cristais=1;
                update_cristal(1, i, j);
            }
            else{
                arena[i][j].cristais = 2;
                update_cristal(2, i, j);
            }

            arena[i][j].terreno=terreno;
        }
    }
}

//system calls
//movimentar os robos
// 0=N,1=NE,2=SE,3=S,4=SW,5=NW
int isvalid(int x,int y){
    if(x>=0 && y>=0 && x<15 && y<15)return 1;
    return 0;
}

//verifica os arredores do robo
void neighborhood(Maquina *m){
    int i=m->x;
    int j=m->y;
    for(int k = 0; k < 6; k++){
        int x = i+movx(j, k);
        int y = j+movy(k);
        if(isvalid(x,y)==0)continue;
        //verifica se ha cristais
        if(arena[x][y].cristais > 0){
            m->vizi_cristais[k] = arena[x][y].cristais;
        }else
            m->vizi_cristais[k] = 0;
        //verifica se há bases (0 -> sem base)
        if(arena[x][y].base > 0){
            m->vizi_bases[k] = arena[x][y].base;
        }else
            m->vizi_bases[k] = 0;
        //verifica se há maquinas
        if(arena[x][y].ocupado > 0){
            if(arena[x][y].base == arena[i][j].base)
                m->vizi_amigos[k] = 1;
            else
                m->vizi_inimigos[k] = 1;
        }else{
            m->vizi_amigos[k] = 0;
            m->vizi_inimigos[k] = 0;
        }

    }
}

//verifica se ha robo do mesmo time em celula adjacente
int hasfriendlyrobot(Maquina *m){
    neighborhood(m);
    for(int i=0;i<6;i++){
        int x = m->x + movx(m->y, i);
        int y = m->y + movy(i);
        if(isvalid(x,y)==0) continue;
        if(m->vizi_amigos[i]==1){
            return 1;
        }
    }
    return 0;
}

//verifica se ha robo de outro time em celula adjacente
int hasenemyrobot(Maquina *m){\
    neighborhood(m);
    for(int i=0;i<6;i++){
        int x = m->x + movx(m->y, i);
        int y = m->y + movy(i);
        if(isvalid(x,y)==0) continue;
        if(m->vizi_inimigos[i]==1){
            return 1;
        }
    }
    return 0;
}

//retorna a posicao de um robo do mesmo time em celula adjacente
int findfriendlyrobot(Maquina *m){
    neighborhood(m);
    for(int i=0;i<6;i++){
        int x = m->x + movx(m->y, i);
        int y = m->y + movy(i);
        if(isvalid(x,y)==0) continue;
        if(m->vizi_amigos[i]==1){
            return i;
        }
    }
    return -1;
}

//verifica se ha robo de outro time em celula adjacente
int findenemyrobot(Maquina *m){
    neighborhood(m);
    for(int i=0;i<6;i++){
        int x = m->x + movx(m->y, i);
        int y = m->y + movy(i);
        if(isvalid(x,y)==0) continue;
        if(m->vizi_amigos[i]==1){
            return i;
        }
    }
    return -1;
}

//move a maquina até coordenada (x,y)
void move(Maquina *soldier, int dir, int n){
    int y = soldier->y;
    int x = soldier->x;
    if(isvalid(x + n*movx(y, dir), y + n*movy(dir))==1){
        if((arena[x + n*movx(y, dir)][y + n*movy(dir)]).ocupado == 0){
            arena[x][y].ocupado = 0;
            update_robot(soldier->id, x, y, x + n*movx(y, dir),
             y + n*movy(dir));
            soldier->x = x + n*movx(y, dir);
            soldier->y = y + n*movy(dir);
            arena[x][y].ocupado = 1;
            neighborhood(soldier);
            printf("Soldado moveu para %d %d \n", (x + movx(y, dir)), (y + movy(dir)));
        }else {printf("%s", "A Celula ja estava ocupada\n");}
    }else {printf("%s", "Movimento fora dos limites da arena.\n");}
}

//ataca a maquina localizada na coordenada (x,y)
void attack(Maquina *soldier, int dir){
    if(isvalid(soldier->x + movx(soldier->y, dir),soldier->y + movy(dir))==1){
        printf("Batalha acontecendo em %d %d \n",(soldier->x + movx(soldier->y, dir)),
            (soldier->y + movy(dir)));
        if(arena[soldier->x + movx(soldier->y, dir)][soldier->y + movy(dir)].ocupado == 1){
            int posx=soldier->x + movx(soldier->y, dir);
            int posy=soldier->y + movy(dir);
            for(int i = 0; i < 1000; i++)
                if(a[i]!=NULL && a[i]->x == posx && a[i]->y == posy){
                    a[i]->vida -= soldier->ataque;
                    if(a[i]->vida <= 0){
                        soldier->cristais+=a[i]->cristais;
                        destroi_maquina(a[i]);
                        a[i] = NULL;
                        break;
                    }
                    else{
                        if(a[i]->cristais>0){
                            a[i]->cristais--;
                            soldier->cristais++;
                        }
                    }
                }
        }else {printf("%s", "Nenhum robô nesse local\n");}
    }else {printf("%s", "Ação fora dos limites da arena.\n");}
}

//recolhe cristal
void retrieve(Maquina *soldier, int dir){
    if(isvalid(soldier->x + movx(soldier->y, dir),soldier->y + movy(dir))==1){
        if((arena[soldier->x + movx(soldier->y, dir)][soldier->y + movy(dir)]).cristais > 0){
            PutCristal(-1, soldier->x + movx(soldier->y, dir), soldier->y + movy(dir));
            soldier->cristais+=1;
            neighborhood(soldier);
            printf("Cristal pegado de %d %d \n",(soldier->x + movx(soldier->y, dir)),
                (soldier->y + movy(dir)));
        }else{printf("%s", "Nao ha cristal nessa posicao\n");}
    }else{printf("%s", "Posicao invalida\n");}
}

//deposita cristal
void put(Maquina *soldier, int dir){
    if(isvalid(soldier->x + movx(soldier->y, dir),soldier->y + movy(dir))==1){
        if(soldier->cristais > 0){
            PutCristal(1, soldier->x + movx(soldier->y, dir), soldier->y + movy(dir));
            soldier->cristais--;
            neighborhood(soldier);
            printf("Cristal depositado em %d %d \n",(soldier->x + movx(soldier->y, dir)),
                (soldier->y + movy(dir)));
        }else{printf("%s", "Seu soldado nao tem cristais\n");}
    }else{printf("%s", "Posicao invalida\n");}
}

//----------------------------------------------------------------------
// TESTES
// 1 : ALEATÓRIO com 1 exército
// 2 : ALEATÓRIO com 2 exércitos
// 3 : ALEATÓRIO com 3 exércitos
// 10: ALEATÓRIO com 10 exércitos
// n : ALEATÓRIO com n exércitos
int TEST = 2;

// N1 movimentos por exército
int N1 = 100;
// Tamanho máximo do salto
int SALTO = 1;
// N2 recolhecimentos de cristal por exército
int N2 = 20;
// N3 depósito de cristal por exército
int N3 = 15;
// N4 ataques por exército
int N4 = 30;
// Proporção de ações para cada tropa do exército
float P[6]  = {0.2, 0.3, 0.1, 0.1, 0.15, 0.15};
// 0 ou 1 para impressão das posições de todas as tropas do exército
int FR = 1;
// 0 ou 1 para impressão da existência de robo inimigo em célula adjacente
int ECR = 1;
// 0 ou 1 para impressão da existência de robo amigo em célula adjacente
int FCR = 1;
// 0 roda as ações dos times de forma alternada, 1 roda todas as ações de um time antes do outro
int TN = 0;

//=========================================================================

void executaAcoes(int exercito, int tropa){
    int r;
    int n1 = 0, n2 = 0, n3 = 0, n4 = 0;
    float c = P[tropa]; 
    int cN1 = (int) (c * N1);
    int cN2 = (int) (c * N2);
    int cN3 = (int) (c * N3);
    int cN4 = (int) (c * N4);

    while (1){
        
        r = rand() % 2;
        
        
        if (r && n1 < cN1){
            move(a[exercito * 6 + tropa], rand() % 6, rand() % SALTO + 1);
            n1++;
        }

        r = rand() % 2;
        if (r && n2 < cN2){
            retrieve(a[exercito * 6 + tropa], rand() % 6);     
            n2++; 
        }

        r = rand() % 2;
         if (r && n3 < cN3){
            
            put(a[exercito * 6 + tropa], rand() % 6);    
            n3++; 
        }

        r = rand() % 2;
        if (r && n4 < cN4){
            attack(a[exercito * 6 + tropa], rand() % 6);
            n4++;
        }

        if (n1 >= cN1 && n2 >= cN2 && n3 >= cN3 && n4 >= cN4)
            break;
        
    }

}


void test(){
    int i, j, k;
    int contador[TEST][6][4];
    // int finished[TEST * 6 * 4];

  //  for (i = 0; i < TEST * 6 * 4; i++)
    //    finished[i] = 0;

    for (i = 0; i < TEST; i++)
        for (j = 0; j < 6; j++)
            for (k = 0; k < 4; k++)
                contador[i][j][k] = 0;

    for (i = 0; i < TEST; i++)
        InsereExercito(i, NULL);
    
    if (TN)
        for (i = 0; i < TEST; i++)
            for (j = 0; j < 6; j++)
                executaAcoes(i, j);
            
    else
        while (1){
            for (i = 0; i < TEST; i++)
                for (j = 0; j < 6; j++){
                    if (contador[i][j][0] < (int)(N1 * P[j])){
                        contador[i][j][0]++;
                        move(a[i * 6 + j], rand() % 6, rand() % SALTO + 1);
                    }
                    //else
                      //  finished[0] = 1;
                    if (contador[i][j][1] < (int)(N2 * P[j])){
                        contador[i][j][1]++;
                        retrieve(a[i * 6 + j], rand() % 6);
                    }
                  //  else
                      //  finished++;
                    if (contador[i][j][2] < (int)(N3 * P[j])){
                        contador[i][j][2]++;
                        put(a[i * 6 + j], rand() % 6);
                    }
                   // else
                     //   finished++;
                    if (contador[i][j][3] < (int)(N4 * P[j])){
                        contador[i][j][3]++;
                        attack(a[i * 6 + j], rand() % 6);
                    }
                   // else
                        //finished++;
                }
            //printf("FINISHED: %d", finished);
          //  if (finished == TEST * 6 * 4){
            //    printf("FIMGAMEOVER");
              //  break;
                
            //}
        }

}

int main(){
    printf("TESTE");
    display = create_display();
    constroi();
    test();
    getchar();
}

// TRATAR:
// Pôr cristal onde tem base
// Remoção do desenho de um cristal quando o robô estiver na célula
// TRATAR:
// Pôr cristal onde tem base
// Remoção do desenho de um cristal quando o robô estiver na célula
// Possível inconsistência de coordenadas do tabuleiro entre Interface e Máquina
