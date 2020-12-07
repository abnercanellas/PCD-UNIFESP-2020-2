#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>

#define SRAND_VALUE 1985
#define DIMENTION 2048 //2048*2048​ e um total de ​2000​ geracoes
#define NGENERATIONS 2000
#define THREADS 2


struct argsCF_t{
    int id, n;
    int* gen; 
};
typedef struct argsCF_t argsCF_t;

struct argsNCS_t{
    int id, i, j;
    int* gen; 
};
typedef struct argsNCS_t argsNCS_t;

pthread_t pt[THREADS];
argsCF_t aCF[THREADS];
argsNCS_t aNCS[THREADS];


void populate(int *grid){ //popula aleatoriamente a matriz
    srand(SRAND_VALUE);
    int i, j;
    for(i = 0; i<DIMENTION; i++) { 
        for(j = 0; j<DIMENTION; j++) {     
            grid[i * DIMENTION + j] = rand() % 2; 
        }
    }
}

void *countFinalCells(void *args){ //Conta o total de celulas vivas no fim da i geração
    argsCF_t *arg =(argsCF_t*) args;
    int i, j;
    arg->n=0;

    for(i=(int)arg->id; i < DIMENTION; i+=THREADS){
        for (j = 0; j < DIMENTION; j++) 
            if((int)arg->gen[i * DIMENTION + j]==1) arg->n++ ;
    }
}

int getNeighbors(int *grid, int i, int j){ //conta numero de vizinhos atualmente vivos
    int n=0, ni = i-1, nj = j-1, pi = i+1, pj = j+1 ; // ni, nj, pi, pj representam i-1, j-1, i+1, j+1

    ni = ni%DIMENTION < 0 ? DIMENTION-1 : ni;
    nj = nj%DIMENTION < 0 ? DIMENTION-1 : nj;
    pi = pi%DIMENTION;
    pj = pj%DIMENTION;

    
    n = grid[ni*DIMENTION +nj] + grid[ni*DIMENTION +j] + grid[ni*DIMENTION +pj] + 
        grid[i*DIMENTION +nj] + grid[i*DIMENTION +pj] + 
        grid[pi*DIMENTION +nj] + grid[pi*DIMENTION +j] + grid[pi*DIMENTION +pj];

    return n;
}


int *newCellState(void *args){//Define o futuro da celula de acordo com as regras
    int n = getNeighbors(gen, i,j); //conta vizinhos

    if(gen[i * DIMENTION + j] == 1){ //se a celula esta viva
        if(n < 2){
            return 0;  //Morre por abandono
        }else if(n == 2 || n == 3){
            return 1;  //Permanece viva
        }else if(n >= 4){
            return 0;  //Morre por superpopulacao
        }
    }
    else if(n==3){ //se a celula esta morta
        return 1;  //Ganha vida
    }
}

void newGen(int *gen, int* auxGen){ //funcao que realiza a atualização da geracao

    for (int t = 0; t < THREADS; t++){
        aNCS[t].id=t;
        aNCS[t].gen=auxGen;
        for(int i=t; i < DIMENTION; i+=THREADS){
            for (int j = 0; j < DIMENTION; j++){
                aNCS[t].i=i;
                aNCS[t].j=j;
                pthread_create(pt[t], NULL, newCellState, (void*)&aNCS[t]);
            }
        }
    }
    for (int t = 0; t < THREADS; t++){
        pthread_join(pt[j],auxGen);
    }
    

    int i,j;
    for(i=0; i < DIMENTION; i++){
        for (j = 0; j < DIMENTION; j++){
            //auxGen[i * DIMENTION + j] =  newCellState(gen, i, j);
        }
    }
    for (i=0;i<(DIMENTION*DIMENTION);i++){
        gen[i]=auxGen[i];
    }
}

int main(){

    struct timeval begin, end;
    gettimeofday(&begin,0);

    
    int total=0;
    int *grid = (int *)malloc(DIMENTION * DIMENTION *sizeof(int)), *auxGrid = (int *)malloc(DIMENTION * DIMENTION *sizeof(int));
    
    populate(grid);

    //countFinalCells(grid, 0);
    //printf("Geracao %d: %d\n", arg.generation, n);
    for(int i = 1; i<=NGENERATIONS; i++){
        total=0;
        newGen(grid, auxGrid); 
        
        for(int t = 0; t<THREADS; t++){
            aCF[t].id=t;
            aCF[t].gen = grid;
            pthread_create(&pt[t],NULL, countFinalCells, (void*)&aCF[t]);
        }
        for(int t = 0; t<THREADS; t++){
            pthread_join(pt[t],NULL);
            total+=aCF[t].n;
        }
        printf("Geracao %d: %d\n", i, total);

        //printf("Geracao %d: %d\n");
    }

    gettimeofday(&end,0);
    long sec = end.tv_sec - begin.tv_sec;
    long mic = end.tv_usec - begin.tv_usec;
    double elap = sec - mic*1e-6;

    printf("Tempo total: %.4f secs", elap);

    return 0;
}
