#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "maq.h"

int tempo=0;
//vetor que armazena cada maquina
Maquina *a[110];
int timestep = 0;
Celula arena[200][200];

//declaro os vetores de movimento
int movx[6]={0,1,1,0,-1,-1};
int movy[6]={-1,-1,0,1,1,0};

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

void Move(Maquina *soldier, int dir, Celula arena, int n){
    if(isvalid(soldier->x + n*movx[dir],soldier->y + n*movy[dir])){
        if((arena[soldier->x + n*movx[dir]][soldier->y + n*movy[dir]]).ocupado == 0){
            arena[soldier->x][soldier->y].ocupado = 0;
            soldier->x = soldier->x + Npasos*movx[dir];
            soldier->y = soldier->y + Npasos*movx[dir];
            arena[soldier->x][soldier->y].ocupado = 1;
        }else {printf("%s", "A Celula ja estava ocupada");}
    }else {printf("%s", "Movimento fora dos limites da arena.");}
}
//recolhe cristal
void Retrieve(Maquina *soldier, int dir, Celula arena){
    if(isvalid(soldier->x + movx[dir],soldier->y + movy[dir])){
        if((arena[soldier->x + movx[dir]][soldier->y + movy[dir]]).cristais > 0){
            (arena[soldier->x + movx[dir]][soldier->y + movy[dir]]).cristais-=1;
            soldier->cristais+=1;
        }else{printf("%s", "Nao ha cristal nessa posicao");}
    }else{printf("%s", "Posicao invalida");}
}
//deposita cristal
void Put(Maquina soldier, int dir, Celula arena){
    if(isvalid(soldier->x + movx[dir],soldier->y + movy[dir])){
        if(soldier->cristais > 0){
            (arena[soldier->x + movx[dir]][soldier->y + movy[dir]]).cristais ++;
            soldier.cristais--;
        }else{printf("%s", "Seu soldado nao tem cristais");}
    }else{printf("%s", "Posicao invalida");}
}

//registro dos ponteiros das maquinas no vetor a[] de robos
void registro_maquina(Maquina *m){
    int i = 0;
    while(a[i] != NULL)
        i++;
    a[i] = m;
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
//remove o exercito x --> n tropas por exercito(nessa fase iremos usar n = 5)
void RemoveExercito(int x){
    for(int i = (x - 1)*5; i < (x * 5); i++)
        destroi_maquina(a[i]);
}

//movimenta cada maquina m (n instruções)
void Atualiza(int n){
    timestep++;
    for(int i = 0; i < 110; i++)
        if(a[i] != NULL)
            exec_maquina(a[i], 50);
        else break;
    
}



