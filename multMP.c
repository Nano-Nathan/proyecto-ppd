// algoritmo para multiplicaciond e matrices usando mpi

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
  int rank, size;
  double tiempo_creation, tiempo_comm, tiempo_calc, start_creation, end_creation, start_calculo, end_calculo, start_comm, end_comm;
  double carga_proceso[10];
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // creamos e inicializamos las matrices
  int filas = atoi(argv[1]);
  int columnas = atoi(argv[2]);
  int filas2da = atoi(argv[3]);
  int i, j, k = 0;

  int **matriz_1;
  int **matriz_2;
  start_creation = MPI_Wtime();
  carga_proceso[0] = MPI_Wtime();
  matriz_2 = (int **)malloc(columnas * sizeof(int *));
  for (i = 0; i < columnas; i++)
  {
    matriz_2[i] = (int *)malloc(filas2da * sizeof(int));
  }

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
    end_creation = MPI_Wtime();
    tiempo_creation = end_creation - start_creation;
    carga_proceso[1] = MPI_Wtime();

    // calculamos el tiempo de ejecucion
    start_calculo = MPI_Wtime();
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
    carga_proceso[1] = MPI_Wtime();
  }
  if (rank == 0)
  {
    start_comm = MPI_Wtime();
  }
  carga_proceso[2] = MPI_Wtime();

  // enviamos con broadcast las matrices a todos los procesos
  MPI_Bcast(&matriz_2[0], columnas * filas2da, MPI_INT, 0, MPI_COMM_WORLD);
  carga_proceso[3] = MPI_Wtime();

  if (rank == 0)
  {
    end_comm = MPI_Wtime();
    tiempo_comm = end_comm - start_comm;
    // dividimos la matriz 1 en vectores que va a recibir cada proceso con isend
    // el proceso 0 se queda con la primera parte de la matriz 1
    int filas_asignadas = filas_por_proceso;
    
      start_comm = MPI_Wtime();
      carga_proceso[4] = MPI_Wtime();
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

      for (j = 0; j < filas_actual; j++)
      {
        MPI_Send(&matriz_1[filas_asignadas + j][0], columnas, MPI_INT, i, j, MPI_COMM_WORLD);
      }
      end_comm = MPI_Wtime();
      tiempo_comm += end_comm - start_comm;
      filas_asignadas += filas_actual;
    }
    carga_proceso[5] = MPI_Wtime();
  }
  else
  {
    carga_proceso[4] = MPI_Wtime();

    // los demas procesos reciben su parte de la matriz 1
    for (j = 0; j < filas_por_proceso; j++)
    {
      MPI_Recv(&matriz_1[j][0], columnas, MPI_INT, 0, j, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    carga_proceso[5] = MPI_Wtime();
  }
  carga_proceso[6] = MPI_Wtime();

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
  carga_proceso[7] = MPI_Wtime();

  // enviamos la porcion de la matriz 1 calculada a cada proceso
  if (rank != 0)
  {
    carga_proceso[8] = MPI_Wtime();

    for (j = 0; j < filas_por_proceso; j++)
    {
      MPI_Send(&resultado[j][0], filas2da, MPI_INT, 0, j, MPI_COMM_WORLD);
    }
    carga_proceso[9] = MPI_Wtime();
  }
  else
  {
    // el proceso 0 recibe las porciones de la matriz 1 calculadas por los demas procesos con irecv
    carga_proceso[8] = MPI_Wtime();

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

      start_comm = MPI_Wtime();
      for (j = 0; j < filas_actual; j++)
      {
        MPI_Irecv(&resultado[filas_asignadas + j][0], filas2da, MPI_INT, i, j, MPI_COMM_WORLD, &request[filas_asignadas + j - filas_por_proceso]);
      }
      end_comm = MPI_Wtime();
      tiempo_comm += end_comm - start_comm;
      filas_asignadas += filas_actual;
    }
    start_comm = MPI_Wtime();
    // verificamos que todos los procesos hayan enviado su parte de la matriz 1
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
    fp = fopen("tiemposParalelos.txt", "a");
    fprintf(fp, "[%d, %d, %d, %d, %d, %f, %f, %f]\n", size, filas, columnas, columnas, filas2da, tiempo_creation, tiempo_comm, tiempo_calc);
    fclose(fp);
  }
  FILE *fpr;
  char buffer[32]; // The filename buffer.
  // Put "file" then k then ".txt" in to filename.
  snprintf(buffer, sizeof(char) * 32, "tiemposNodo%i.txt", rank);
  fpr = fopen(buffer, "a");
  fprintf(fpr, "[");
  for(i=0; i<10; i++){
    fprintf(fpr, "%f,", carga_proceso[i] - carga_proceso[0]);
  }
  fprintf(fpr, "]\n");
  fclose(fpr);
  MPI_Finalize();
  return 0;
}
