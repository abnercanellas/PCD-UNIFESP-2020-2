package JavaThread;

import java.util.Random;
import java.lang.Thread;

//Lembrar de compilar com javac JavaThread/jogoDaVidaJavaThread.java 
// e exec com java JavaThread.jogoDaVidaJavaThread 

public class jogoDaVidaJavaThread {

    static private int DIMENTION = 2048; //2048*2048​ e um total de ​2000​ geracoes
    static private int NGENERATIONS = 2000;
    static private int THREADS = 8;
    
    static private countFinalCellsThread[] cFCT = new countFinalCellsThread[THREADS];
    static private newGenThread[] nGT = new newGenThread[THREADS];
    static private Thread[] th = new Thread[THREADS*2];

    static private void populate(int[] grid){
        Random gerador = new Random(1985);
        int i, j;
        for(i = 0; i<DIMENTION; i++) {
                for(j = 0; j<DIMENTION; j++){
                    grid[i * DIMENTION +j] = gerador.nextInt(2147483647) % 2;
            }
        }
    }

    static private void countFinalCells(int[] grid, int generation){
        int n=0;

        for(int t = 0; t < THREADS; t++){
            cFCT[t] = new countFinalCellsThread(grid, t, DIMENTION, THREADS);
            th[t] = new Thread(cFCT[t]);
            th[t].start();
        }
        for(int t = 0; t < THREADS; t++){
            try {
                th[t].join();
                n+=cFCT[t].getN();
            } catch (InterruptedException e) {
                System.out.println(e.getMessage());
            }
        }
        System.out.println("Geracao " + generation + ": " + n);
    }

    static private int[] newGen(int[] gen, int[] auxGen){

        for(int t = 0; t< THREADS; t++){
            nGT[t] = new newGenThread(gen, auxGen, t, DIMENTION, THREADS);
            th[t*2] = new Thread(nGT[t]);
            th[t*2].start();
        }
        for(int t = 0; t < THREADS; t++){
            try {
                th[t*2].join();
                for(int i=t; i < DIMENTION; i+=THREADS){
                    for (int j = 0; j < DIMENTION; j++){
                        auxGen[i * DIMENTION +j] = nGT[t].getNauxGen()[i * DIMENTION +j];
                    }
                }
            } catch (InterruptedException e) {
                System.out.println(e.getMessage());
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
        //System.out.println("Tempo total: " + elapsed + "ms");
    }

}