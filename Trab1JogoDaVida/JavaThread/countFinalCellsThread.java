package JavaThread;

public class countFinalCellsThread implements Runnable{
    private int[] cgrid;
    private int thId, Dim , nThreads, n=0;
    public countFinalCellsThread(int[] grid, int thID, int DIM, int NTHREADS){
        cgrid = grid;
        thId = thID;
        Dim = DIM;
        nThreads = DIM<NTHREADS ? DIM : NTHREADS;
    }
    public void run() {
        for (int j = thId; j < (Dim*Dim); j+=nThreads) {
            if(cgrid[j]==1) n++ ;
        }
    }
    public int getN() {
        return n;
    }
}
