# Proyecto final para Programacion paralela y distribuida
## Integrantes:
- **Cristian Miguel SERRANO**
- **Gastón Ezequiel CAVALLO**

## Compilación
Para compilar el programa paralelo se debe correr el siguiente comando:
```bash
$ mpicc multMP.c -o mainParallel
``` 
## Ejecución
Para ejecutar el programa paralelo en lotes se debe correr el siguiente comando:
```bash
$ source ./executeParallel.sh
```
Para ejecutar el programa secuencial en lotes se debe correr el siguiente comando:
```bash
$ source ./executeSec.sh
```
Para una simple ejecución del programa paralelo se debe correr el siguiente comando:
```bash
$ mpiexec -n 4 ./mainParallel 1000 1000 1000
```
donde el primer parámetro es la cantidad de filas de la matriz A, el segundo parámetro es la cantidad de columnas de la matriz A y el tercer parámetro es la cantidad de columnas de la matriz B. La cantidad de columnas de la matriz A esta dada por la cantidad de filas de la matriz B.

