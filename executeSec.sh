#!/bin/bash

matriz_1=(200 400 500 1000 1000 2000 2000 3000 4000)
matriz_2=(300 400 500 500 1000 2000 3000 3000 4000)
matriz_3=(400 400 500 500 1000 2000 4000 3000 4000)
# for loop to iterate over the arrays
for i in {1..9}
do
  echo "Starting ${matriz_1[$i]} ${matriz_2[$i]} ${matriz_3[$i]}"
  # Execute task 1
  mpiexec -n 1 ./mainSec ${matriz_1[$i]} ${matriz_2[$i]} ${matriz_3[$i]}
  echo "Task ${i} completed."

  # Wait for task 1 to finish before starting task 2
  echo "Waiting for task to finish..."
  wait
done

echo "All tasks completed."