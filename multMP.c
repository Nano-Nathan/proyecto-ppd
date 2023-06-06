// algoritmo para multiplicaciond e matrices usando mpi

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // creamos e inicializamos las matrices
  int filas = atoi(argv[1]);
  int columnas = atoi(argv[2]);
  int filas2da = atoi(argv[3]);
  // printf("Matriz 1: %d x %d, Matriz 2: %d x %d\n", filas, columnas, columnas, filas2da);
  // printf("Proceso %d de %d\n", rank, size);
  int i, j, k = 0;

  int **matriz_1;

  int **matriz_2;
  matriz_2 = (int **)malloc(columnas * sizeof(int *));
  for (i = 0; i < columnas; i++)
  {
    matriz_2[i] = (int *)malloc(filas2da * sizeof(int));
  }

  double tiempo;
  struct timeval start_mpi, end_mpi;
  int **resultado;
  // calculo para saber que filas le tocan a cada proceso
  int filas_por_proceso = filas / size;
  int filas_sobrantes = filas % size;
  if (rank < filas_sobrantes + 1 && rank != 0)
  {
    filas_por_proceso++;
  }
  // printf("Proceso %d de %d, filas: %d\n", rank, size, filas_por_proceso);

  if (rank == 0)
  {
    // inicializa las matrices
    matriz_1 = (int **)malloc(filas * sizeof(int *));
    resultado = (int **)malloc(filas * sizeof(int *));

    for (i = 0; i < filas; i++)
    {
      matriz_1[i] = (int *)malloc(columnas * sizeof(int));
      resultado[i] = (int *)malloc(filas2da * sizeof(int));
    }

    // ponemos valores iniciales aleatorios a las matrices 1 y 2
    for (i = 0; i < filas; i++)
    {
      for (j = 0; j < columnas; j++)
      {
        matriz_1[i][j] = (int)rand() / (int)(RAND_MAX / 10.0);
      }
    }
    for (i = 0; i < columnas; i++)
    {
      for (j = 0; j < filas2da; j++)
      {
        matriz_2[i][j] = (int)rand() / (int)(RAND_MAX / 10.0);
      }
    }
    // imprimimos la matriz 2
    printf("Matriz 2:\n");
    for (i = 0; i < columnas; i++)
    {
      for (j = 0; j < filas2da; j++)
      {
        printf(".%d ", matriz_2[i][j]);
      }
      printf("\n");
    }
    // calculamos el tiempo de ejecucion
    gettimeofday(&start_mpi, NULL);
  }
  else
  {
    // reserva espacio de las matrices en cada proceso
    matriz_1 = (int **)malloc(filas_por_proceso * sizeof(int *));
    resultado = (int **)malloc(filas_por_proceso * sizeof(int *));
    for (i = 0; i < filas_por_proceso; i++)
    {
      matriz_1[i] = (int *)malloc(columnas * sizeof(int));
      resultado[i] = (int *)malloc(filas2da * sizeof(int));
    }
  }
  // enviamos con broadcast las matrices a todos los procesos
  MPI_Bcast(&matriz_2[0], columnas * filas2da, MPI_INT, 0, MPI_COMM_WORLD);
  // MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // printf("soy proceso %d y sobrevivi al bcast y tengo %d filas\n", rank, filas_por_proceso);

  if (rank == 0)
  {
    MPI_Request request[size];
    // imprimimos la matriz 1
    printf("Matriz 1:\n");
    for (i = 0; i < filas; i++)
    {
      for (j = 0; j < columnas; j++)
      {
        printf(".%d ", matriz_1[i][j]);
      }
      printf("\n");
    }
    // dividimos la matriz 1 en vectores que va a recibir cada proceso con isend
    // el proceso 0 se queda con la primera parte de la matriz 1
    int filas_asignadas = filas_por_proceso;
    for (i = 1; i < size; i++)
    {
      // printf("Proceso %d de %d recibio filas\n", i, size);
      // printf("Proceso %d recibio %d elementos\n", i, filas_por_proceso * columnas);
      // imprimimos los datos a enviar
      // for (j = filas_asignadas; j < filas_asignadas + filas_por_proceso; j++)
      // {
      //   for (k = 0; k < columnas; k++)
      //   {
      //     printf(".%d ", matriz_1[j][k]);
      //   }
      //   printf("\n");
      // }
      int filas_actual = 0;
      if (i < filas_sobrantes + 1)
      {
        filas_actual = filas_por_proceso + 1;
      }
      else
      {
        filas_actual = filas_por_proceso;
      }
      // printf("Proceso %d recibio desde %d hasta %d\n", i, filas_asignadas, filas_asignadas + filas_actual);

      // MPI_Send(&matriz_1[filas_asignadas][0], filas_por_proceso * columnas, MPI_INT, i, 0, MPI_COMM_WORLD);
      for (j = 0; j < filas_actual; j++)
      {
        MPI_Send(&matriz_1[filas_asignadas + j][0], columnas, MPI_INT, i, j, MPI_COMM_WORLD);
      }
      filas_asignadas += filas_actual;
    }

    // verificamos que todos los procesos hayan recibido su parte de la matriz 1
    // MPI_Waitall(size, request, MPI_STATUS_IGNORE);
    // printf("Proceso 0 envio la matriz 1 correctamente\n");
  }
  else
  {
    // los demas procesos reciben su parte de la matriz 1
    for (j = 0; j < filas_por_proceso; j++)
    {
      MPI_Recv(&matriz_1[j][0], columnas, MPI_INT, 0, j, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    if (rank == 1)
    {
      // printf("Soy Proceso %d de %d recibio filas\n", rank, size);
      // imprimimos las filas recibidas
      // for (i = 0; i < filas_por_proceso; i++)
      // {
      //   for (j = 0; j < columnas; j++)
      //   {
      //     printf("%d ", matriz_1[i][j]);
      //   }
      //   printf("\n");
      // }
      // imprimimos la matriz 2
      // printf("Matriz 2:\n");
      // for (i = 0; i < columnas; i++)
      // {
      //   for (j = 0; j < filas2da; j++)
      //   {
      //     printf(".%d ", matriz_2[i][j]);
      //   }
      //   printf("\n");
      // }
    }
  }
  // printf("soy proceso %d y me saltee una parte xd\n", rank);
  // hacemos el calculo de la multiplicacion de matrices
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
  // printf("soy proceso %d y calcule la matriz\n", rank);
  // enviamos la porcion de la matriz 1 calculada a cada proceso
  if (rank != 0)
  {
    for (j = 0; j < filas_por_proceso; j++)
    {
      MPI_Send(&resultado[j][0], filas2da, MPI_INT, 0, j, MPI_COMM_WORLD);
    }

    // printf("soy proceso %d y envie el dato\n", rank);
  }
  else
  {
    // el proceso 0 recibe las porciones de la matriz 1 calculadas por los demas procesos con irecv

    MPI_Request request[filas - filas_por_proceso];
    int filas_asignadas = filas_por_proceso;
    for (i = 1; i < size; i++)
    {

      int filas_actual = 0;
      if (i < filas_sobrantes + 1)
      {
        filas_actual = filas_por_proceso + 1;
      }
      else
      {
        filas_actual = filas_por_proceso;
      }
      // printf("De Proceso %d voy a recibir desde %d hasta %d\n", i, filas_asignadas, filas_asignadas + filas_actual);

      // MPI_Send(&matriz_1[filas_asignadas][0], filas_por_proceso * columnas, MPI_INT, i, 0, MPI_COMM_WORLD);
      for (j = 0; j < filas_actual; j++)
      {
        MPI_Irecv(&resultado[filas_asignadas + j][0], filas2da, MPI_INT, i, j, MPI_COMM_WORLD, &request[filas_asignadas + j - filas_por_proceso]);
      }
      filas_asignadas += filas_actual;
    }

    // verificamos que todos los procesos hayan enviado su parte de la matriz 1
    MPI_Waitall(filas - filas_por_proceso, request, MPI_STATUS_IGNORE);
    // printf("Proceso 0 recibio la matriz resultado correctamente\n");
    // calculamos el tiempo de ejecucion
    gettimeofday(&end_mpi, NULL);
    tiempo = (end_mpi.tv_sec - start_mpi.tv_sec) + (end_mpi.tv_usec - start_mpi.tv_usec) / 1000000.0;
    printf("Tiempo de ejecucion: %f ms\n", tiempo);
    // mostramos las matrices originales y el resultado
    // printf("Matriz 1:\n");
    // for (i = 0; i < filas; i++)
    // {
    //   for (j = 0; j < columnas; j++)
    //   {
    //     printf("%d ", matriz_1[i][j]);
    //   }
    //   printf("\n");
    // }
    // printf("Matriz 2:\n");
    // for (i = 0; i < columnas; i++)
    // {
    //   for (j = 0; j < filas2da; j++)
    //   {
    //     printf("%d ", matriz_2[i][j]);
    //   }
    //   printf("\n");
    // }
    // printf("Matriz resultado:\n");
    // for (i = 0; i < filas; i++)
    // {
    //   for (j = 0; j < filas2da; j++)
    //   {
    //     printf("%d ", resultado[i][j]);
    //   }
    //   printf("\n");
    // }
  }

  if (rank == 0)
  {
    // guardamos un archivo con los tiempos de ejecucion y los tamannos de las matrices
    FILE *fp;
    fp = fopen("tiemposParalelos.txt", "a");
    fprintf(fp, "%d %d %d %f\n", filas, columnas, filas2da, tiempo);
    fclose(fp);
  }

  MPI_Finalize();
  return 0;
}
