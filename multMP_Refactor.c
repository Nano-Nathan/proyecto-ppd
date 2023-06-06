// algoritmo para multiplicaciond e matrices usando mpi

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int i, j, k;
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Arreglo que guardará la cantidad de filas que le toca a cada proceso
    int count_filas[size];

    // Obtenemos las dimensiones de las matrices
    int filas = atoi(argv[1]);
    int columnas = atoi(argv[2]);
    int filas2da = atoi(argv[3]);

    // Inicializamos variables de tiempo
    double tiempo;
    struct timeval start_mpi, end_mpi;

    // Inicializamos variables de matrices
    int **matriz_1;
    int **matriz_2;
    int **resultado;

    // Reservamos espacio de memoria para matriz 2
    matriz_2 = (int **)malloc(columnas * sizeof(int *));
    for (i = 0; i < columnas; i++)
    {
        matriz_2[i] = (int *)malloc(filas2da * sizeof(int));
    }

    // Calculamos las filas que le tocan al proceso actual
    int filas_por_proceso = filas / size;
    int filas_sobrantes = filas % size;
    if (rank < filas_sobrantes + 1 && rank != 0)
    {
        filas_por_proceso++;
    }

    if (rank == 0)
    {
        int filas_asignadas;

        // Reserva espacio de memoria para la matriz 1 y matriz resultado
        matriz_1 = (int **)malloc(filas * sizeof(int *));
        resultado = (int **)malloc(filas * sizeof(int *));
        for (i = 0; i < filas; i++)
        {
            matriz_1[i] = (int *)malloc(columnas * sizeof(int));
            resultado[i] = (int *)malloc(filas2da * sizeof(int));
        }

        // Inicializamos aleatoriamente la matriz 1
        for (i = 0; i < filas; i++)
        {
            for (j = 0; j < columnas; j++)
            {
                matriz_1[i][j] = (int)rand() / (int)(RAND_MAX / 10.0);
            }
        }

        // Inicializamos aleatoriamente la matriz 2
        for (i = 0; i < columnas; i++)
        {
            for (j = 0; j < filas2da; j++)
            {
                matriz_2[i][j] = (int)rand() / (int)(RAND_MAX / 10.0);
            }
        }

        // calculamos el tiempo de ejecucion
        gettimeofday(&start_mpi, NULL); // usamos MPI_Wtime?

        // Guarda el índice de la última fila asignada (El proceso 0 se queda con las primeras)
        filas_asignadas = filas_por_proceso;
        count_filas[0] = filas_por_proceso;

        // Envía las filas a cada proceso
        for (i = 1; i < size; i++)
        {

            // Calcula la cantidad de filas asignadas al proceso i
            int filas_actual = 0;
            if (i < filas_sobrantes + 1)
            {
                filas_actual = filas_por_proceso + 1;
            }
            else
            {
                filas_actual = filas_por_proceso;
            }

            // Guarda la cantidad de filas asignadas al proceso
            count_filas[i] = filas_actual;

            // Envía las filas a cada proceso
            for (j = 0; j < filas_actual; j++)
            {
                MPI_Send(&matriz_1[filas_asignadas + j][0], columnas, MPI_INT, i, j, MPI_COMM_WORLD);
            }

            // Se aumenta el índice de las filas asignadas
            filas_asignadas += filas_actual;
        }
    }
    else
    {
        // Reserva espacio de las filas correspondientes en cada proceso
        matriz_1 = (int **)malloc(filas_por_proceso * sizeof(int *));
        resultado = (int **)malloc(filas_por_proceso * sizeof(int *));
        for (i = 0; i < filas_por_proceso; i++)
        {
            matriz_1[i] = (int *)malloc(columnas * sizeof(int));
            resultado[i] = (int *)malloc(filas2da * sizeof(int));
        }

        // Los demas procesos reciben su parte de la matriz 1
        for (j = 0; j < filas_por_proceso; j++)
        {
            MPI_Recv(&matriz_1[j][0], columnas, MPI_INT, 0, j, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    // Enviamos con broadcast las matriz 2 a todos los procesos
    MPI_Bcast(&matriz_2[0], columnas * filas2da, MPI_INT, 0, MPI_COMM_WORLD);

    // Resolvemos la sección correspondiente
    for (i = 0; i < filas_por_proceso; i++)
    {
        for (j = 0; j < filas2da; j++)
        {
            for (k = 0; k < columnas; k++)
            {
                resultado[i][j] += matriz_1[i][k] * matriz_2[k][j];
            }
        }
    }

    // Enviamos la porcion de la matriz 1 calculada al proceso 0
    if (rank != 0)
    {
        for (i = 0; i < filas_por_proceso; i++)
        {
            MPI_Send(&resultado[i][0], filas2da, MPI_INT, 0, i, MPI_COMM_WORLD);
        }
    }
    else
    {
        // El proceso 0 recibe las porciones de la matriz resultado calculadas por los demas procesos
        MPI_Request request[filas - filas_por_proceso];

        // Contador que guarda la posicion de la fila actual asignada
        int filas_asignadas = count_filas[0];
        for (i = 1; i < size; i++)
        {
            // Recibe las filas con una llamada no bloqueante
            for (j = 0; j < count_filas[i]; j++)
            {
                MPI_Irecv(&resultado[filas_asignadas + j][0], filas2da, MPI_INT, i, j, MPI_COMM_WORLD, &request[filas_asignadas + j - filas_por_proceso]);
            }
            filas_asignadas += count_filas[i];
        }

        // Verificamos que todos los procesos hayan enviado su parte de la matriz resultado
        MPI_Waitall(filas - filas_por_proceso, request, MPI_STATUS_IGNORE);

        // Calculamos el tiempo de ejecucion
        gettimeofday(&end_mpi, NULL); // usamos MPI_Wtime?
        tiempo = (end_mpi.tv_sec - start_mpi.tv_sec) + (end_mpi.tv_usec - start_mpi.tv_usec) / 1000000.0;
        printf("Tiempo de ejecucion: %f ms\n", tiempo);

        // Mostramos las matrices originales y el resultado
        // printf("Matriz 1:\n");
        // for (i = 0; i < filas; i++)
        // {
        //     for (j = 0; j < columnas; j++)
        //     {
        //         printf("%d ", matriz_1[i][j]);
        //     }
        //     printf("\n");
        // }
        // printf("Matriz 2:\n");
        // for (i = 0; i < columnas; i++)
        // {
        //     for (j = 0; j < filas2da; j++)
        //     {
        //         printf("%d ", matriz_2[i][j]);
        //     }
        //     printf("\n");
        // }
        // printf("Matriz resultado:\n");
        // for (i = 0; i < filas; i++)
        // {
        //     for (j = 0; j < filas2da; j++)
        //     {
        //         printf("%d ", resultado[i][j]);
        //     }
        //     printf("\n");
        // }
    }

    if (rank == 0)
    {
        // guardamos un archivo con los tiempos de ejecucion y los tamannos de las matrices
        FILE *fp;
        fp = fopen("tiemposParalelosRefactor.txt", "a");
        fprintf(fp, "%d %d %d %f\n", filas, columnas, filas2da, tiempo);
        fclose(fp);
    }

    MPI_Finalize();
    return 0;
}
