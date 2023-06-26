#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define DEBUG 1            // comentar esta linha quando for medir tempo
#define ARRAY_SIZE 48 
#define PORCENTAGEM 0.5     // trabalho final com o valores 10.000, 100.000, 1.000.000

int my_rank;
int proc_n;

void bs(int n, int * vetor)
{
    int c=0, d, troca, trocou =1;

    while (c < (n-1) & trocou )
        {
        trocou = 0;
        for (d = 0 ; d < n - c - 1; d++)
            if (vetor[d] > vetor[d+1])
                {
                troca      = vetor[d];
                vetor[d]   = vetor[d+1];
                vetor[d+1] = troca;
                trocou = 1;
                }
        c++;
        }
}

int main(int argc, char* argv[])
{
    MPI_Status status;

    MPI_Init(&argc , &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); // pega pega o numero do processo atual (rank)
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);  // pega informacao do numero de processos (quantidade total)

    int oks[proc_n];
    int partialSize = ARRAY_SIZE/proc_n;
    int extra = partialSize*PORCENTAGEM;
    int vetor[partialSize+extra];
    int maxVal = (partialSize) * (proc_n - my_rank); 

    for (int i=0 ; i<partialSize; i++){              /* init array with worst case for sorting */
        vetor[i] = maxVal-i;
    }
    int pronto = 0;
    int maiorMsg;
    while(pronto == 0) {
        bs(partialSize, vetor);   
        if(my_rank != proc_n-1) {
            MPI_Send ( &vetor[partialSize-1], 1, MPI_INT, my_rank+1, 0, MPI_COMM_WORLD);
        }
        if(my_rank!=0) {
            MPI_Recv(&maiorMsg, 1, MPI_INT, my_rank-1, 0, MPI_COMM_WORLD, &status);
            if(vetor[0]>=maiorMsg){
                oks[my_rank] = 1;
            }else {
                oks[my_rank] = 0;
            }
        }

        for (int i = 0; i < proc_n; i++) {
            MPI_Bcast(&oks[i], 1, MPI_INT, i, MPI_COMM_WORLD);
        }

        pronto = 1;
        for (int i = 0; i < proc_n; i++) {
            if(oks[i] == 0) {
                pronto = 0;
            }
        }
        if(pronto == 1) {
            break;
        }

        if(my_rank!=0) {
            MPI_Send (&vetor[0], extra, MPI_INT, my_rank-1, 1, MPI_COMM_WORLD);
        }        
        if(my_rank != proc_n-1) {
            MPI_Recv(&vetor[partialSize], extra, MPI_INT, my_rank+1, 1, MPI_COMM_WORLD, &status);

            int highVet[extra * 2];
            for(int i = 0; i < extra * 2; i++) {
                highVet[i] = vetor[partialSize - extra + i];
            }

            bs(extra*2, highVet);

            
            for(int i = 0; i < extra; i++) {
                vetor[partialSize-extra+i] = highVet[i]; 
            }
            
            MPI_Send ( &highVet[extra], extra, MPI_INT, my_rank+1, 2, MPI_COMM_WORLD);
        }

        if (my_rank != 0) {
            MPI_Recv(&vetor[0], extra, MPI_INT, my_rank-1, 2, MPI_COMM_WORLD, &status);
        }
    }

    #ifdef DEBUG
    printf("\nVetor de %d: ", my_rank);
    for (int i=0 ; i<partialSize; i++)                              /* print sorted array */
        printf("[%d] ", vetor[i]);
    printf("\n");
    #endif
    MPI_Finalize();
    return 0;
}
