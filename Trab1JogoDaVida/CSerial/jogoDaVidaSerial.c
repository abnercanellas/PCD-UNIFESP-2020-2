#include <stdlib.h>
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
    int n=0, i, j;
    
    for(i=0; i < DIMENTION; i++){
        for (j = 0; j < DIMENTION; j++) 
            if(grid[i * DIMENTION + j]==1) n++ ;
    }
    
    printf("Geracao %d: %d\n", generation, n);
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

    
    //teste "Vizinhos por ponto" (Liberar print corresp em Life() )
    /* printf("(%d,%d) | (%d,%d) | (%d,%d)        %d | %d | %d\n(%d,%d) |   %d   | (%d,%d)        %d | %d | %d\n(%d,%d) | (%d,%d) | (%d,%d)        %d | %d | %d\n",
     ni,nj, ni,j, ni,pj, grid[ni*DIMENTION +nj], grid[ni*DIMENTION +j], grid[ni*DIMENTION +pj], 
     i,nj, n ,i,pj, grid[i*DIMENTION +nj], grid[i*DIMENTION+j], grid[i*DIMENTION +pj] , 
     pi,nj, pi,j, pi,pj, grid[pi*DIMENTION +nj], grid[pi*DIMENTION +j], grid[pi*DIMENTION +pj]); */

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

    int i,j;
    for(i=0; i < DIMENTION; i++){
        for (j = 0; j < DIMENTION; j++){
            auxGen[i * DIMENTION + j] =  newCellState(gen, i, j);
        }
    }
    for (i=0;i<(DIMENTION*DIMENTION);i++){
        gen[i]=auxGen[i];
    }
}

int main(){

    //declara wallTime pra teste de performance
    struct timeval begin, end;
    //inicia a contagem de wallTime
    gettimeofday(&begin,0);
    

    //fiz a alocacao em 1 vetor para melhorar a performance de acesso em mem
    //acesso a grid[i][j] deve ser feito por grid[i * DIMENTION + j]
    int *grid = (int *)malloc(DIMENTION * DIMENTION *sizeof(int)), *auxGrid = (int *)malloc(DIMENTION * DIMENTION *sizeof(int));
    
    populate(grid);

    countFinalCells(grid, 0);
    //printField(grid);
    for(int i = 1; i<=NGENERATIONS; i++){
        newGen(grid, auxGrid); //optei por criar a matriz auxiliar aqui pra só ocorrer uma alocação dela, ao invez de uma alocação por for
        countFinalCells(grid, i);
        //printField(grid);
        //getNeighbors(grid, 2, 2); //teste "Vizinhos por ponto" (Liberar print corresp em getNeighbors() )
    }
    //countFinalCells(grid, NGENERATIONS);

    //finaliza e printa a contagem de wallTime
    gettimeofday(&end,0);
    long sec = end.tv_sec - begin.tv_sec;
    long mic = end.tv_usec - begin.tv_usec;
    double elap = sec - mic*1e-6;

    printf("Tempo total: %.4f secs", elap);

    return 0;
}
