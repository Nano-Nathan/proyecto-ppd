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
    // Inicializamos variables de tiempo
    double tiempo_creation, tiempo_comm, tiempo_calc, start_creation, end_creation, start_calculo, end_calculo, start_comm, end_comm;
    double carga_proceso[10];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Arreglo que guardará la cantidad de filas que le toca a cada proceso
    int count_filas[size];

    // Obtenemos las dimensiones de las matrices
    int filas = atoi(argv[1]);
    int columnas = atoi(argv[2]);
    int filas2da = atoi(argv[3]);

    // Inicializamos variables de matrices
    int **matriz_1;
    int **matriz_2;
    int **resultado;
    start_creation = MPI_Wtime();
    carga_proceso[0] = MPI_Wtime();

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

        end_creation = MPI_Wtime();
        tiempo_creation = end_creation - start_creation;
        carga_proceso[1] = MPI_Wtime();

        // calculamos el tiempo de ejecucion
        start_calculo = MPI_Wtime();

        // Guarda el índice de la última fila asignada (El proceso 0 se queda con las primeras)
        filas_asignadas = filas_por_proceso;
        count_filas[0] = filas_por_proceso;

        start_comm = MPI_Wtime();
        carga_proceso[2] = MPI_Wtime();
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

            end_comm = MPI_Wtime();
            tiempo_comm += end_comm - start_comm;
            // Se aumenta el índice de las filas asignadas
            filas_asignadas += filas_actual;
        }

        carga_proceso[3] = MPI_Wtime();
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
        carga_proceso[1] = MPI_Wtime();

        carga_proceso[2] = MPI_Wtime();
        // Los demas procesos reciben su parte de la matriz 1
        for (j = 0; j < filas_por_proceso; j++)
        {
            MPI_Recv(&matriz_1[j][0], columnas, MPI_INT, 0, j, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        carga_proceso[3] = MPI_Wtime();
    }
    carga_proceso[4] = MPI_Wtime();

    if (rank == 0)
    {
        start_comm = MPI_Wtime();
    }
    carga_proceso[5] = MPI_Wtime();
    // Enviamos con broadcast las matriz 2 a todos los procesos
    MPI_Bcast(&matriz_2[0], columnas * filas2da, MPI_INT, 0, MPI_COMM_WORLD);

    carga_proceso[6] = MPI_Wtime();
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

    carga_proceso[7] = MPI_Wtime();
    // Enviamos la porcion de la matriz 1 calculada al proceso 0
    if (rank != 0)
    {
        carga_proceso[8] = MPI_Wtime();
        for (i = 0; i < filas_por_proceso; i++)
        {
            MPI_Send(&resultado[i][0], filas2da, MPI_INT, 0, i, MPI_COMM_WORLD);
        }
        carga_proceso[9] = MPI_Wtime();
    }
    else
    {
        carga_proceso[8] = MPI_Wtime();
        // El proceso 0 recibe las porciones de la matriz resultado calculadas por los demas procesos
        MPI_Request request[filas - filas_por_proceso];

        // Contador que guarda la posicion de la fila actual asignada
        int filas_asignadas = count_filas[0];
        start_comm = MPI_Wtime();
        for (i = 1; i < size; i++)
        {
            // Recibe las filas con una llamada no bloqueante
            for (j = 0; j < count_filas[i]; j++)
            {
                MPI_Irecv(&resultado[filas_asignadas + j][0], filas2da, MPI_INT, i, j, MPI_COMM_WORLD, &request[filas_asignadas + j - filas_por_proceso]);
            }
            end_comm = MPI_Wtime();
            tiempo_comm += end_comm - start_comm;
            filas_asignadas += count_filas[i];
        }
        start_comm = MPI_Wtime();

        // Verificamos que todos los procesos hayan enviado su parte de la matriz resultado
        MPI_Waitall(filas - filas_por_proceso, request, MPI_STATUS_IGNORE);

        end_comm = MPI_Wtime();
        tiempo_comm += end_comm - start_comm;
        carga_proceso[9] = MPI_Wtime();

        end_calculo = MPI_Wtime();
        tiempo_calc = end_calculo - start_calculo - tiempo_comm;
        printf("Tiempo de ejecucion: %f s\n", tiempo_calc);
        printf("Tiempo de comunicacion: %f s\n", tiempo_comm);
        printf("Tiempo de creacion: %f s\n", tiempo_creation);
    }

    if (rank == 0)
    {
        // guardamos un archivo con los tiempos de ejecucion y los tamannos de las matrices
        FILE *fp;
        fp = fopen("tiemposParalelosRefactor.txt", "a");
        fprintf(fp, "[%d, %d, %d, %d, %d, %f, %f, %f]\n", size, filas, columnas, columnas, filas2da, tiempo_creation, tiempo_comm, tiempo_calc);
        fclose(fp);
    }

    FILE *fpr;
    char buffer[32]; // The filename buffer.
    // Put "file" then k then ".txt" in to filename.
    snprintf(buffer, sizeof(char) * 32, "tiemposNodo%iR.txt", rank);
    fpr = fopen(buffer, "a");
    fprintf(fpr, "[");
    for (i = 0; i < 10; i++)
    {
        fprintf(fpr, "%f,", carga_proceso[i] - carga_proceso[0]);
    }
    fprintf(fpr, "]\n");
    fclose(fpr);
    MPI_Finalize();
    return 0;
}
