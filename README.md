# Proyecto final para Programacion paralela y distribuida
## Integrantes:
- **Cristian Miguel SERRANO**
- **Gastón Ezequiel CAVALLO**

## Descripción
Diseñar y desarrollar un algoritmo paralelo que resuelva la multiplicación de matrices, con la forma C = A∗B, con A y B no necesariamente cuadradas. Realizar la entrega correspondiente al diseño paralelo a seguir, a través del Aula Abierta.

## Algoritmo secuencial
Implementamos algoritmo basico para multiplicacion de matrices, el cual consta de 3 bucles for iterando filas de la primer matriz, columnas de la primer matriz y filas de la segunda matriz respectivamente. En cada iteracion se realiza la multiplicacion de los elementos correspondientes y se suma al resultado parcial.

### Tabla de resultados

| Tamaño de matriz A | Tamaño de matriz B | Tiempo de ejecución (ms) |
|--------------------|--------------------|---------------------|
| 200 x 300          | 300 x 400          | 88.000000            |
| 2000 x 3000        | 3000 x 4000        | 288846.000000        | 

## Algoritmo paralelo
Implementamos un algoritmo que consta de la reparticion a los nodos worker de las filas de la primer matriz y las columnas de la segunda, luego cada worker realiza la multiplicacion de las filas y columnas correspondientes y envia el resultado parcial al nodo master, el cual se encarga de sumar los resultados parciales y almacenarlos en la matriz resultado.

### Tabla de resultados

| Tamaño de matriz A | Tamaño de matriz B | Tiempo de ejecución (ms) |
|--------------------|--------------------|---------------------|
| 200 x 300          | 300 x 400          | 88.000000            |
| 2000 x 3000        | 3000 x 4000        | 288846.000000        | 

## Metricas
### Speedup


### Eficiencia


## Conclusiones
