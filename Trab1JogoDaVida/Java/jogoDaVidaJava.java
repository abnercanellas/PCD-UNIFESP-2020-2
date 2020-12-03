package Trab1JogoDaVida.Java;

import java.util.Random;

public class jogoDaVidaJava {

    private int DIMENTION = 10; //2048*2048​ e um total de ​2000​ geracoes
    //private int NGENERATIONS = 2048;

    public void populate(Integer[][] grid){
        Random gerador = new Random(1985);
        for(int i = 0; i<DIMENTION; i++) { //laço sobre as células do tabuleiro sem contar comum eventual halo
                for(int j = 0; j<DIMENTION; j++){
                    grid[i][j] = gerador.nextInt(2147483647) % 2;
            }
        }
    }

    public static void main(String[] args) {
        //populate();
        //countFinalCells();
        for(int i=1; i<10; i++){
            //newGen();
            //countFinalCells();
        }
    }

}