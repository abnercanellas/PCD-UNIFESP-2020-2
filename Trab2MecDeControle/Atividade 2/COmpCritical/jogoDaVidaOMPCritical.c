#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <sys/time.h>

#define SRAND_VALUE 1985
#define DIMENTION 2048 //2048*2048​ e um total de ​2000​ geracoes
#define NGENERATIONS 2000
#define THREADS 8

int total=0;

void populate(int *grid){ //popula aleatoriamente a matriz
    srand(SRAND_VALUE);
    int i, j;
    for(i = 0; i<DIMENTION; i++) { 
        for(j = 0; j<DIMENTION; j++) {     
            grid[i * DIMENTION + j] = rand() % 2; 
        }
    }
}

void countFinalCells(int *grid, int generation){ //Conta o total de celulas vivas no fim da i geração
    //struct timeval begin, end;
    //gettimeofday(&begin,0);

    int n=0, i, j;
    #pragma omp parallel shared(grid) private(i,j) num_threads(THREADS)
    {  
        #pragma omp for
        for(i=0; i < DIMENTION; i++){
            for (j = 0; j < DIMENTION; j++) 
                if(grid[i * DIMENTION + j]==1) 
                    #pragma omp critical(foo) //uso de critical para proteger a variavel n
                    {
                        total++ ;
                    }
        }
    }
    printf("Geracao %d: %d\n", generation, total);

    // gettimeofday(&end,0);
    // long sec = end.tv_sec - begin.tv_sec;
    // long mic = end.tv_usec - begin.tv_usec;
    // double elap = sec + mic*1e-6;
    // printf("Tempo total: %.4f sec\n", elap);
    
    // Serial/1Thread func=0.0087s real=2m28,298s
    // 2Threads func=0.0097s real=1m31,553s
    // 4Threads func=0.0232s real=1m6,505s
    // 8Threads func=0.0293s real=1m4,742s  
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

int newCellState(int* gen, int i, int j){//Define o futuro da celula de acordo com as regras
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

    int i,j;
    #pragma omp parallel shared(gen, auxGen) private(i,j) num_threads(THREADS) 
    {
        #pragma omp for
        for(i=0; i < DIMENTION; i++){
            for (j = 0; j < DIMENTION; j++){
                auxGen[i * DIMENTION + j] =  newCellState(gen, i, j);
        }
    }
    }
    for (i=0;i<(DIMENTION*DIMENTION);i++){
        gen[i]=auxGen[i];
    }
}

int main(){

    struct timeval begin, end;
    gettimeofday(&begin,0);

    int *grid = (int *)malloc(DIMENTION * DIMENTION *sizeof(int)), *auxGrid = (int *)malloc(DIMENTION * DIMENTION *sizeof(int));
    
    populate(grid);
    int i;
    // countFinalCells(grid, 0);
    for( i= 1; i<=NGENERATIONS; i++){
        newGen(grid, auxGrid);
        // countFinalCells(grid, i);
        printf("%d\n",i);
    }
    countFinalCells(grid, i-1);

    // gettimeofday(&end,0);
    // long sec = end.tv_sec - begin.tv_sec;
    // long mic = end.tv_usec - begin.tv_usec;
    // double elap = sec - mic*1e-6;

    //printf("Tempo total: %.4f sec", elap);

    return 0;
}
