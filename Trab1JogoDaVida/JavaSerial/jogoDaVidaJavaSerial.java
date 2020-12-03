package Trab1JogoDaVida.JavaSerial;

import java.util.Random;

public class jogoDaVidaJavaSerial {

    static private int DIMENTION =2048; //2048*2048​ e um total de ​2000​ geracoes
    static private int NGENERATIONS = 2000;

    static private void populate(int[] grid){
        Random gerador = new Random(1985);

        int i, j;
        for(i = 0; i<DIMENTION; i++) { //laço sobre as células do tabuleiro sem contar comum eventual halo
                for(j = 0; j<DIMENTION; j++){
                    grid[i * DIMENTION +j] = gerador.nextInt(2147483647) % 2;
            }
        }
    }

    static private void countFinalCells(int[] grid, int generation){
        int n=0;
    
        for (int i : grid) {
            if(i==1) n++ ;
        }
        
        System.out.println("Geracao " + generation + ": " + n);
    }

    static private int getNeighbors(int[] grid, int i, int j){
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

    /* static private void printField(int[] grid, int generation){
        int i, j, n=0, m=0, v=0;
        for (i=0; i<DIMENTION;i++) {
            for(j=0; j<DIMENTION; j++){
                System.out.print("| " + grid[i * DIMENTION + j] + " "); 
            }
            //Teste Printa matriz de vizinhos e soma mortos+nascidos
            System.out.print("|         ");        
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
                System.out.print("| " + n + " - " + grid[offset] + " "); //vai printar | nVizinhos - statusCelula na tabela da 
            }
            System.out.println("|   m: " + m + " v: " + v);
        }
    } */

    static private int newCellState(int[] gen, int i, int j){
        int n = getNeighbors(gen, i, j);
        int k=0;

        if(gen[i * DIMENTION + j] == 1){ //se a celula esta viva
            if(n < 2){
                k = 0;  //Morre por abandono
            }else if(n == 2 || n == 3){
                k =  1;  //Permanece viva
            }else if(n >= 4){
                k =  0;  //Morre por superpopulacao
            }
        }
        else if(n==3){ //se a celula esta morta
            k =  1;  //Ganha vida
        }
        return k;
    }

    static private int[] newGen(int[] gen, int[] auxGen){

        int i,j;
        for(i=0; i < DIMENTION; i++){
            for (j = 0; j < DIMENTION; j++){
                int offset = i * DIMENTION + j;
                auxGen[offset] =  newCellState(gen, i, j);
            }
        }
        return auxGen;
    }

    public static void main(String[] args) {

        long startTime = System.nanoTime();

        int[] grid = new int[DIMENTION * DIMENTION];
        int[] auxGrid = new int[DIMENTION * DIMENTION];

        populate(grid);
        
        //printField(grid,0);
        countFinalCells(grid, 0);

        for(int i=1; i<=NGENERATIONS; i++){
            grid = newGen(grid, auxGrid).clone();
            countFinalCells(grid,i);
            //printField(grid,i);
        }

        long endTime = System.nanoTime();
        long elapsed = (endTime - startTime) / 1000000;
        System.out.println("Tempo total: " + elapsed + "ms");
    }

}