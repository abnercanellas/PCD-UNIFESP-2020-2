/* 
    integrantes:
        Abner Y. D. C. Canellas, RA: 150738;
        Henrique D C Silveira, RA: 93930;
        Jonatas da Silva Oliveira, RA: 99640;
        Raphael Gomes Autuori, RA:120569.
 */
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <sys/time.h>

#define SRAND_VALUE 1985
#define DIMENTION 2048 //2048*2048​ e um total de ​2000​ geracoes
#define NGENERATIONS 2000

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
    int n=0, total=0, i, j, myId, nProc;
    MPI_Comm_size( MPI_COMM_WORLD , &nProc);
    MPI_Comm_rank( MPI_COMM_WORLD , &myId);

    
    for(i=myId; i < DIMENTION; i+=nProc){
        for (j = 0; j <  DIMENTION; j++) 
            if(grid[i * DIMENTION + j]==1) n++ ;
    }

    MPI_Reduce( &n , &total , 1 , MPI_INT , MPI_SUM , 0 , MPI_COMM_WORLD); //recebe os n locais e soma salvando o total em total no proc de rank 0

    if(myId==0){
        printf("Geracao %d: %d\n", generation, total);
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

    int i, j, myId, nProc;

    MPI_Comm_size( MPI_COMM_WORLD , &nProc);
    MPI_Comm_rank( MPI_COMM_WORLD , &myId);

    for(i=myId; i < DIMENTION; i+=nProc){ //cada rank calcula os novos estados de celula da sua porção
        for (j = 0; j < DIMENTION; j++){
            auxGen[i * DIMENTION + j] =  newCellState(gen, i, j);
        }
    }

    if(myId!=0){
        MPI_Send( auxGen , (DIMENTION * DIMENTION) , MPI_INT , 0 , 1 , MPI_COMM_WORLD); // os workers enviam sua variavel auxgen pro master(proc 0)
    }
    if(myId==0){
        for(i=0; i < DIMENTION; i+=nProc){ //copia a porção que o master processou para prox geracao
            for (j = 0; j < DIMENTION; j++){
                gen[i * DIMENTION + j] = auxGen[i * DIMENTION + j] ;
            }
        }
        for(int h=1; h<nProc ; h++){
            MPI_Recv( auxGen, (DIMENTION * DIMENTION) , MPI_INT , h , 1 , MPI_COMM_WORLD , NULL); //master recebe o auxgen dos workers
            for(i=h; i < DIMENTION; i+=nProc){ //copia a porcao de cada worker para a prox geracao
                for (j = 0; j < DIMENTION; j++){
                    gen[i * DIMENTION + j] = auxGen[i * DIMENTION + j] ;
                }
            }
        }
    }
    MPI_Bcast( gen , (DIMENTION * DIMENTION), MPI_INT , 0 , MPI_COMM_WORLD); //compartilha nova gen entre os procs
}

int main(int argc, char* argv[]){ //main

    MPI_Init(&argc, &argv);
    //i * DIMENTION + j
    int *grid = (int *)malloc(DIMENTION * DIMENTION *sizeof(int)), *auxGrid = (int *)malloc(DIMENTION * DIMENTION *sizeof(int));
    int i;

    populate(grid);

    for(i = 1; i<=NGENERATIONS; i++){
        newGen(grid, auxGrid);
        countFinalCells(grid, i);
    }
    MPI_Finalize();

    return 0;
}