#include <stdio.h>
#include <stdlib.h>

#define DEBUG 1            // comentar esta linha quando for medir tempo
#define ARRAY_SIZE 80      // trabalho final com o valores 10.000, 100.000, 1.000.000

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

int main()
{
    int my_rank;
    int proc_n;
    int pronto = 0;

    MPI_Init();
    MPI_Comm_rank(&my_rank);
    MPI_Comm_size(&proc_n);
    MPI_Status status;

    int partialSize = ARRAY_SIZE/proc_n;
    int maxVal = (partialSize) * (my_rank+1);
    int vetor[partialSize];
    int i;
    int message;
    int okVizinho = 0;


    for (i=0 ; i<partialSize; i++)              /* init array with worst case for sorting */
        vetor[i] = maxVal-i;
   

    while(!pronto) {
        bs(partialSize, vetor);    
        if(my_rank<proc_n-1){
            MPI_Send(vetor[partialSize-1], 1, MPI_INT, my_rank+1, 0, MPI_COMM_WORLD);
        }
        if(my_rank!=0) {
            MPI_Recv(&message, 1, MPI_INT, my_rank-1, 1, MPI_COMM_WORLD, &status);
        }
        if(vetor[0]>message) {
            okVizinho = 1;
        }
        for (size_t i = 0; i < proc_n; i++)
        {
            MPI_Bcast(&okVizinho, 1, MPI_INT, my_rank, MPI_COMM_WORLD);
            if(okVizinho == 1) {
                pronto = 1;
            }
        }
        
    }
    #ifdef DEBUG
    printf("\nVetor: ");
    for (i=0 ; i<ARRAY_SIZE; i++)              /* print unsorted array */
        printf("[%03d] ", vetor[i]);
    #endif

                     /* sort array */


    #ifdef DEBUG
    printf("\nVetor: ");
    for (i=0 ; i<ARRAY_SIZE; i++)                              /* print sorted array */
        printf("[%03d] ", vetor[i]);
    #endif

    return 0;
}