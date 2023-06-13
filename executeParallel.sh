#!/bin/bash

processors=(2 4)
matriz_1=(200 400 500 1000 1000 2000 2000 3000 4000)
matriz_2=(300 400 500 500 1000 2000 3000 3000 4000)
matriz_3=(400 400 500 500 1000 2000 4000 3000 4000)
# for loop to gather 3 executions
for j in {1..3}
do
  # for loop to test with different number of processors
  for k in {0..1}
  do
    # for loop to iterate over the arrays
    for i in {0..8}
    do
      echo "Starting ${matriz_1[$i]} ${matriz_2[$i]} ${matriz_3[$i]}"
      # Execute task 1
      mpiexec -n ${processors[$k]} ./mainParallel ${matriz_1[$i]} ${matriz_2[$i]} ${matriz_3[$i]}
      echo "Task ${i} completed."

      # Wait for task 1 to finish before starting task 2
      echo "Waiting for task to finish..."
      wait
    done
    # for loop to iterate over the arrays
    for i in {0..8}
    do
      echo "Starting ${matriz_1[$i]} ${matriz_2[$i]} ${matriz_3[$i]}"
      # Execute task 1
      mpiexec -n ${processors[$k]} ./mainParallelR ${matriz_1[$i]} ${matriz_2[$i]} ${matriz_3[$i]}
      echo "Task ${i} completed."

      # Wait for task 1 to finish before starting task 2
      echo "Waiting for task to finish..."
      wait
    done
  done
done

echo "All tasks completed."