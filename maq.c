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
Maquina *a[1000];
//vetor que armazena os pares (x,y) das bases
int bases[1000][2];
Celula arena[15][15];

//declaro os vetores de movimento
int movx[6]={0,1,1,0,-1,-1};
int movy[6]={-1,-1,0,1,1,0};

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
Maquina *cria_maquina(INSTR *p) {
    srand(time(NULL));
    int coordX = rand()%15;
    int coordY = rand()%15;
    Maquina *m = (Maquina*)malloc(sizeof(Maquina));
    if (!m) Fatal("Memória insuficiente",4);
    while(arena[coordX][coordY].ocupado){
        coordX = rand()%15;
        coordY = rand()%15;
    }
    m->x = coordX;
    m->y = coordY;
    m->ataque = 30;
    m->vida = 100;
    m->cristais = 0;
    m->ip = 0;
    m->prog = p;
    arena[coordX][coordY].ocupado = 1;
    return m;
}

void destroi_maquina(Maquina *m) {
    update_robot(m->id, m->x, m->y, -1, -1);
    free(m);
}

//faz a base do time z no par (x,y)
void fazbase(int x,int y,int z){
    while(arena[x][y].ocupado == 1 || arena[x][y].base != 0 || arena[x][y].cristais){
        srand(time(NULL));
        int coordX = rand()%15;
        int coordY = rand()%15;
        x = coordX;
        y = coordY;
    }
    bases[z][0] = x;
    bases[z][1] = y;
    arena[x][y].terreno = 3;
    arena[x][y].base = z;
    arena[x][y].ocupado = 1;
    update_base("BASE_", z, x, y);
}

// Adiciona o x-ésimo exercito
// Vincula uma base para cada membro de exercito
// Exercito = grupo de robos/tropas
void InsereExercito(int x, int tropas, INSTR *p){
    srand(time(NULL));
    int coordX = rand()%15;
    int coordY = rand()%15;
    fazbase(coordX, coordY, x);
    for(int j = 0; j < tropas; j++){
        Maquina *maq;
        maq = cria_maquina(p);
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
void vizinhanca(Maquina *m){
    int i=m->x;
    int j=m->y;
    for(int k = 0; k < 6; k++){
        int x = i+movx[k];
        int y = j+movy[k];
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

//move a maquina até coordenada (x,y)
void Move(Maquina *soldier, int dir, int n){
    if(isvalid(soldier->x + n*movx[dir],soldier->y + n*movy[dir])==1){
        if((arena[soldier->x + n*movx[dir]][soldier->y + n*movy[dir]]).ocupado == 0){
            arena[soldier->x][soldier->y].ocupado = 0;
            update_robot(soldier->id, soldier->x, soldier->y, soldier->x + n*movx[dir],
             soldier->y + n*movy[dir]);
            soldier->x = soldier->x + n*movx[dir];
            soldier->y = soldier->y + n*movy[dir];
            arena[soldier->x][soldier->y].ocupado = 1;
            vizinhanca(soldier);
            printf("Soldado moveu para %d %d \n",(soldier->x + movx[dir]),
                (soldier->y + movy[dir]));
        }else {printf("%s", "A Celula ja estava ocupada\n");}
    }else {printf("%s", "Movimento fora dos limites da arena.\n");}
}

//ataca a maquina localizada na coordenada (x,y)
void Attack(Maquina *soldier, int dir){
    if(isvalid(soldier->x + movx[dir],soldier->y + movy[dir])==1){
        printf("Batalha acontecendo em %d %d \n",(soldier->x + movx[dir]),
            (soldier->y + movy[dir]));
        if((arena[soldier->x + movx[dir]][soldier->y + movy[dir]]).ocupado == 1){
            int posx=soldier->x + movx[dir];
            int posy=soldier->y + movy[dir];
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
    }else {printf("%s", "Movimento fora dos limites da arena.\n");}
}

//recolhe cristal
void Retrieve(Maquina *soldier, int dir){
    if(isvalid(soldier->x + movx[dir],soldier->y + movy[dir])==1){
        if((arena[soldier->x + movx[dir]][soldier->y + movy[dir]]).cristais > 0){
            PutCristal(-1, soldier->x + movx[dir], soldier->y + movy[dir]);
            soldier->cristais += 1;
            vizinhanca(soldier);
            printf("Cristal pegado de %d %d \n",(soldier->x + movx[dir]),
                (soldier->y + movy[dir]));
        }else{printf("%s", "Nao ha cristal nessa posicao\n");}
    }else{printf("%s", "Posicao invalida\n");}
}

//deposita cristal
void Put(Maquina *soldier, int dir){
    if(isvalid(soldier->x + movx[dir],soldier->y + movy[dir])==1){
        if(soldier->cristais > 0){
            PutCristal(1, soldier->x + movx[dir], soldier->y + movy[dir]);
            soldier->cristais--;
            vizinhanca(soldier);
            printf("Cristal depositado em %d %d \n",(soldier->x + movx[dir]),
                (soldier->y + movy[dir]));
        }else{printf("%s", "Seu soldado nao tem cristais\n");}
    }else{printf("%s", "Posicao invalida\n");}
}

//pequena main para testes

int main(){
    printf("TESTE");
    display = create_display();
    constroi();
    // 2 Exércitos - E1, E2
    // N1 número de tropas de E1 (1-5)
    // N2 número de tropas de E2 (1-5)
    // X ações de E1 ou E2
    // P tropa atual
    int N1 = 1 + rand() % 3;
    int N2 = 1 + rand() % 3;
    int X  = 1 + rand() % 100;
    InsereExercito(0, N1, NULL);
    InsereExercito(1, N2, NULL);
    for (int i = 0; i < X; i++){
        int P = rand() % (N1 + N2);
        Move(a[P], rand() % 6, 1);
        Retrieve(a[P], rand() % 6);
        Put(a[P], rand() % 6);
        Attack(a[P], rand() % 6);
    }
    getchar();
}



// TRATAR:
// Pôr cristal onde tem base
// Remoção do desenho de um cristal quando o robô estiver na célula
// Possível inconsistência de coordenadas do tabuleiro entre Interface e Máquina