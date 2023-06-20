#include <stdio.h>
#include <stdlib.h>

#define DEBUG 1            // comentar esta linha quando for medir tempo
#define ARRAY_SIZE 80      // trabalho final com o valores 10.000, 100.000, 1.000.000
#define PORCENTAGEM 0.3

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

int tudoOk(int vetor[], int size) {
    for (size_t i = 0; i < size; i++)
    {
        if(vetor[i] == 0) {
            return 0;
        }
    }
    return 1;
    
}

void mandarParaEsquerda(int elevet[], int size, int destino){
    int tamanhoCorte = size * PORCENTAGEM;
    int messageVet[tamanhoCorte];
    for (int i = 0; i < tamanhoCorte; i++)
    {
        elevet[i] = messageVet[i];
    }
    MPI_Send(messageVet, tamanhoCorte, MPI_INT, destino, 2, MPI_COMM_WORLD);
}

void receberDaDireita(int source, int size){
    int messageVet[tamanhoCorte];
    MPI_Send(messageVet, tamanhoCorte, MPI_INT, destino, 2, MPI_COMM_WORLD);
    for (int i = 0; i < tamanhoCorte; i++)
    {
        elevet[i] = messageVet[i];
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
    int oks[proc_n];


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
            oks[my_rank] = okVizinho;
        }
        for (size_t i = 0; i < proc_n; i++)
        {
            MPI_Bcast(oks[my_rank], 1, MPI_INT, my_rank, MPI_COMM_WORLD);
        }
        if(tudoOk(oks, proc_n)) {
                pronto = 1;
                break;
        }else {
            if(my_rank != 0) {
                mandarParaEsquerda(vetor, partialSize, my_rank-1);
            }
            if(my_rank!= proc_n-1) {
                receberDaDireita(vetor, partialSize, my_rank-1);//recebo os menores valores da direita                }
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