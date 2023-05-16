// algoritmo secuencial de multiplicacion de matrices en c

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

// // funcion para imprimir las matrices
// void printMatriz(int matriz, int filas, int columnas){
//   int i,j;
//   for(i=0;i<filas;i++){
//     for(j=0;j<columnas;j++){
//       printf("%f ", matriz[i][j]);
//     }
//     printf("\n");
//   }
// }

// funcion main, toma la cantidad de filas y columnas de las matrices, genera dos matrices aleatorias y calcula su producto
int main(int argc, char** argv) {
  
  int filas = atoi(argv[1]);
  int columnas = atoi(argv[2]);
  int filas2da = atoi(argv[3]);
  printf("Matriz 1: %d x %d, Matriz 2: %d x %d\n", filas, columnas, columnas, filas2da);
  int i,j,k=0;
  int** matriz_1 ;

  int** matriz_2;

  int** resultado;

  struct timeval start, end;
  double tiempo;

  // inicializa las matrices
  matriz_1 = (int **)malloc(filas*sizeof(int*));
  matriz_2 = (int **)malloc(columnas*sizeof(int*));
  resultado = (int **)malloc(filas*sizeof(int*));

  for(i=0;i<filas;i++){
    matriz_1[i] = (int *)malloc(columnas*sizeof(int));
    resultado[i] = (int *)malloc(filas2da*sizeof(int));
  }
  for(i=0;i<columnas;i++){
    matriz_2[i] = (int *)malloc(filas2da*sizeof(int));
  }

  // ponemos valores iniciales aleatorios a las matrices 1 y 2
  for(i=0;i<filas;i++){
    for(j=0;j<columnas;j++){
      matriz_1[i][j] = (int)rand()/(int)(RAND_MAX/10.0);
    }
  }
  for(i=0;i<columnas;i++){
    for(j=0;j<filas2da;j++){
      matriz_2[i][j] = (int)rand()/(int)(RAND_MAX/10.0);
    }
  }
  
  // imprimimos las matrices
  // printf("matriz 1:\n");
  // printMatriz(matriz_1, filas, columnas);
  // printf("matriz 2:\n");
  // printMatriz(matriz_2, columnas, filas2da);
  // calculamos el producto de las matrices
  gettimeofday(&start, NULL);
  for(i=0;i<filas;i++){
    for(j=0;j<filas2da;j++){
      resultado[i][j] = 0;
      for(k=0;k<columnas;k++){
        resultado[i][j] += matriz_1[i][k]*matriz_2[k][j];
      }
    }
  }
  gettimeofday(&end, NULL);
  tiempo = (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000;
  printf("Tiempo de ejecucion: %f ms\n", tiempo);
  // imprimimos el resultado
  // printf("resultado:\n");
  // printMatriz(&&resultado, filas, filas2da);
  // liberamos la memoria
  for(j=0;j<columnas;j++){
    free(matriz_1[i]);
  }
  for(j=0;j<filas2da;j++){
    free(matriz_2[i]);
    free(resultado[i]);
  }
  
  free(matriz_1);
  free(matriz_2);
  free(resultado);
  return 0;
}
