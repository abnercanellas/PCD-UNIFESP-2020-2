package JavaThread;

public class newGenThread implements Runnable{
    private int[] ngen, nauxGen;
    private int dim, nth, thId;
    public newGenThread(int[] gen, int[] auxGen, int thID, int DIM, int nTreads){
        ngen = gen;
        nauxGen = auxGen;
        dim = DIM;
        thId = thID;
        nth = nTreads>DIM ? DIM : nTreads;
    }
    public void run(){
        for(int i=thId; i < dim; i+=nth){
            for (int j = 0; j < dim; j++){
                nauxGen[i * dim + j] =  newCellState(ngen, i, j);
            }
        }
    }

    private int newCellState(int[] gen, int i, int j){
        int n = getNeighbors(gen, i, j);
        int k=0;

        if(gen[i * dim + j] == 1){ //se a celula esta viva
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

    private int getNeighbors(int[] grid, int i, int j){
        int n=0, ni = i-1, nj = j-1, pi = i+1, pj = j+1 ; // ni, nj, pi, pj representam i-1, j-1, i+1, j+1

        ni = ni%dim < 0 ? dim-1 : ni;
        nj = nj%dim < 0 ? dim-1 : nj;
        pi = pi%dim;
        pj = pj%dim;
    
        
        n = grid[ni*dim +nj] + grid[ni*dim +j] + grid[ni*dim +pj] + 
            grid[i*dim +nj] + grid[i*dim +pj] + 
            grid[pi*dim +nj] + grid[pi*dim +j] + grid[pi*dim +pj];

        return n;
    }

    public int[] getNauxGen() {
        return nauxGen;
    }

}
