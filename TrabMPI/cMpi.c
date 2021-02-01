#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <sys/time.h>

#define SRAND_VALUE 1985
#define DIMENTION 2048 //2048*2048​ e um total de ​2000​ geracoes
#define NGENERATIONS 20
/* Geracao 19: 701293
Geracao 20: 690063
 */
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
    int n=0, t=0, i, j, myId, nProc;
    MPI_Comm_size( MPI_COMM_WORLD , &nProc);
    MPI_Comm_rank( MPI_COMM_WORLD , &myId);

    
    for(i=myId; i < DIMENTION; i+=nProc){
        for (j = 0; j <  DIMENTION; j++) 
            if(grid[i * DIMENTION + j]==1) n++ ;
    }

    MPI_Reduce( &n , &t , 1 , MPI_INT , MPI_SUM , 0 , MPI_COMM_WORLD);
    
    MPI_Barrier( MPI_COMM_WORLD);

    if(myId==0){
        printf("Geracao %d: %d\n", generation, t);
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

void printField(int *grid){ //printar o "mundo"
    int i,j,n=0,m=0,v=0;
    for(i=0; i < DIMENTION; i++){
        for (j = 0; j < DIMENTION; j++){
            printf("| %d ", grid[i * DIMENTION + j]);
        }
        //Teste Printa matriz de vizinhos e soma mortos+nascidos
        printf("|         ");        
        for (j = 0; j < DIMENTION; j++){
            int offset = i * DIMENTION + j;
            n = getNeighbors(grid,i,j);
            if(grid[offset]==1){ //se a celula esta viva
                if(n < 2){
                    m++;  //Morre por abandono
                }else if(n>=4){
                    m++;  //Morre por superpopulacao
                }
            }
            else if(n==3){ //se a celula esta morta
                v++;  //Ganha vida
            }
            printf("| %d - %d ", n,grid[offset]); //vai printar | nVizinhos - statusCelula na tabela da 
        }
        printf("|  m: %d v: %d\n", m, v);
    }
    printf("morrem: %d nascidos: %d\n----------------------------------------------\n", m,v);
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

    int *recGen = (int *)malloc(DIMENTION * DIMENTION *sizeof(int));

    MPI_Comm_size( MPI_COMM_WORLD , &nProc);
    MPI_Comm_rank( MPI_COMM_WORLD , &myId);

    for(i=myId; i < DIMENTION; i+=nProc){
        for (j = 0; j < DIMENTION; j++){
            auxGen[i * DIMENTION + j] =  newCellState(gen, i, j);
        }
    }

    if(myId!=0){
        MPI_Send( auxGen , (DIMENTION * DIMENTION) , MPI_INT , 0 , 1 , MPI_COMM_WORLD);
    }
    if(myId==0){
        for(i=0; i < DIMENTION; i+=nProc){//copia a porção que o master processou para prox geracao
            for (j = 0; j < DIMENTION; j++){
                gen[i * DIMENTION + j] = auxGen[i * DIMENTION + j] ;
            }
        }
        for(int h=1; h<nProc ; h++){
            MPI_Recv( recGen, (DIMENTION * DIMENTION) , MPI_INT , h , 1 , MPI_COMM_WORLD , NULL);
            for(i=h; i < DIMENTION; i+=nProc){
                for (j = 0; j < DIMENTION; j++){
                    gen[i * DIMENTION + j] = recGen[i * DIMENTION + j] ;
                }
            }
        }
    }
    MPI_Bcast( gen , (DIMENTION * DIMENTION), MPI_INT , 0 , MPI_COMM_WORLD);
}

int main(int argc, char* argv[]){

    MPI_Init(&argc, &argv);
      
    int *grid = (int *)malloc(DIMENTION * DIMENTION *sizeof(int)), *auxGrid = (int *)malloc(DIMENTION * DIMENTION *sizeof(int));
    int i;

    populate(grid);

    //countFinalCells(grid, 0);
    //printField(grid);
    for(i = 1; i<=NGENERATIONS; i++){
        newGen(grid, auxGrid);
        countFinalCells(grid, i);
    }
    //countFinalCells(grid, i);
    MPI_Finalize();

    return 0;
}
