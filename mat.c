#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define DEBUG 1 // comentar esta linha quando for medir tempo
#define ARRAY_SIZE 800
#define PORCENTAGEM 0.5 // trabalho final com o valores 10.000, 100.000, 1.000.000

int my_rank;
int proc_n;

int DefineTamanhoLocal(int rankProcesso, int numeroProcessos)
{
    int tamanhoLocal = ARRAY_SIZE / numeroProcessos;
    int resto = ARRAY_SIZE % numeroProcessos;
    if (rankProcesso < resto)
        tamanhoLocal++;

    return tamanhoLocal;
}
void Mostra(int *vetor, int tamanho)
{
    printf("\nVetor de %d: ", my_rank);
    for (int i = 0; i < tamanho; i++) /* print sorted array */
        printf("[%03d] ", vetor[i]);
    printf("\n");
}

void bs(int n, int *vetor)
{
    int c = 0, d, troca, trocou = 1;

    while (c < (n - 1) & trocou)
    {
        trocou = 0;
        for (d = 0; d < n - c - 1; d++)
            if (vetor[d] > vetor[d + 1])
            {
                troca = vetor[d];
                vetor[d] = vetor[d + 1];
                vetor[d + 1] = troca;
                trocou = 1;
            }
        c++;
    }
}

int main(int argc, char *argv[])
{
    MPI_Status status;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); // pega pega o numero do processo atual (rank)
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);  // pega informacao do numero de processos (quantidade total)

    int oks[proc_n];
    oks[0] = 1;
    int partialSize = DefineTamanhoLocal(my_rank, proc_n);
    int extra = partialSize * PORCENTAGEM;
    int *vetor = (int *)malloc((ARRAY_SIZE) * sizeof(int));
    int maxVal = (partialSize) * (proc_n - my_rank);
    int tamanhoRecebido;
    int *bufferRecebimento;
    bufferRecebimento = (int *)malloc(ARRAY_SIZE * sizeof(int));

    for (int i = 0; i < partialSize; i++)
    { /* init array with worst case for sorting */
        vetor[i] = maxVal - i;
    }
    int pronto = 0;
    int maiorMsg;
    while (pronto == 0)
    {
        bs(partialSize, vetor);
        if (my_rank != proc_n - 1)
        {
            MPI_Send(&vetor[partialSize - 1], 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
        }
        if (my_rank != 0)
        {
            MPI_Recv(&maiorMsg, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, &status);
            if (vetor[0] >= maiorMsg)
            {
                oks[my_rank] = 1;
            }
            else
            {
                oks[my_rank] = 0;
            }
        }

        for (int i = 0; i < proc_n; i++)
        {
            MPI_Bcast(&oks[i], 1, MPI_INT, i, MPI_COMM_WORLD);
        }

        pronto = 1;
        for (int i = 0; i < proc_n; i++)
        {
            if (oks[i] == 0)
            {
                pronto = 0;
            }
        }
        if (pronto == 1)
        {
            break;
        }

        if (my_rank != 0)
        {
            MPI_Send(&vetor[0], extra, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD);
        }
        if (my_rank != proc_n - 1)
        {
            int direita = my_rank + 1;
            MPI_Recv(bufferRecebimento, ARRAY_SIZE, MPI_INT, direita, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &tamanhoRecebido);

            for (int i = 0; i < tamanhoRecebido; i++)
                vetor[partialSize + i] = bufferRecebimento[i];

            bs(tamanhoRecebido * 2, &vetor[partialSize - tamanhoRecebido]);
            MPI_Send(&vetor[partialSize], tamanhoRecebido, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
        }

        if (my_rank != 0)
        {
            MPI_Recv(bufferRecebimento, ARRAY_SIZE, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &tamanhoRecebido);
            for (int i = 0; i < tamanhoRecebido; i++)
                vetor[i] = bufferRecebimento[i];
        }
    }

    Mostra(vetor, partialSize);
    MPI_Finalize();
    return 0;
}
