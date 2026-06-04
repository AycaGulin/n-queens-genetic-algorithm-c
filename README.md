# N-Queens Genetic Algorithm Solver in C

## Project Description

This project is a C-based implementation of the N-Queens problem using a genetic algorithm. The N-Queens problem requires placing N queens on an N by N chessboard so that no two queens attack each other.

The program uses an evolutionary search approach to find a valid board configuration. It represents candidate solutions as chromosomes, evaluates their fitness, selects parents, applies crossover and mutation, and evolves the population until a solution is found or the maximum generation limit is reached.

The project demonstrates important algorithmic concepts such as genetic algorithms, optimization, permutation-based representation, fitness evaluation, tournament selection, crossover, mutation, elitism, dynamic memory allocation, and board visualization.

## Technologies Used

- C Programming Language
- Standard C Libraries
- GCC Compiler
- Genetic Algorithm

## Project Structure

```text
n-queens-genetic-algorithm-c
├── .gitignore
├── README.md
└── src
    └── n_queens_problem.c
```

## Problem Definition

The N-Queens problem is a classic constraint satisfaction and optimization problem. The goal is to place N queens on an N by N chessboard so that:

- No two queens are in the same row
- No two queens are in the same column
- No two queens are on the same diagonal

The program supports board sizes from:

```text
N = 4 to N = 20
```

## Genetic Algorithm Approach

### Chromosome Representation

Each candidate solution is represented as a one-dimensional array.

```text
genes[column] = row
```

For example, if:

```text
genes[0] = 2
```

this means that the queen in column 0 is placed in row 2.

The program uses a permutation-based representation. This means each row value is used only once, so row and column conflicts are automatically avoided. As a result, the genetic algorithm only needs to focus on diagonal conflicts.

### Fitness Function

The fitness function evaluates how good a board configuration is. Since the representation prevents row and column conflicts, the fitness function counts diagonal conflicts between queens.

The maximum possible fitness is calculated as:

```text
N * (N - 1) / 2
```

A perfect solution is reached when there are no diagonal conflicts and the fitness value equals the maximum possible fitness.

### Selection

The program uses tournament selection. A small number of individuals are randomly selected from the population, and the one with the highest fitness is chosen as a parent.

### Crossover

The program uses a prefix-fill crossover method. A random cut point is selected, part of the first parent is copied into the child, and the remaining positions are filled using the second parent while avoiding duplicate row values.

This helps preserve the permutation structure of the chromosome.

### Mutation

The program uses swap mutation. Two random positions in the chromosome are selected, and their values are swapped.

Mutation introduces variation into the population and helps the algorithm avoid getting stuck in local optima.

### Elitism

The best individual from the current generation is copied directly into the next generation. This ensures that the best solution found so far is not lost during evolution.

## Parameters Used

The program uses the following fixed parameters:

```text
Population size: 200
Maximum generations: 2000
Mutation rate: 0.05
Tournament size: 3
```

These values were selected to balance solution quality and runtime.

## Features

- Solves the N-Queens problem using a genetic algorithm
- Supports board sizes from 4 to 20
- Uses permutation-based chromosome representation
- Evaluates fitness based on diagonal conflicts
- Uses tournament selection
- Uses prefix-fill crossover
- Uses swap mutation
- Applies elitism
- Prints the final board using `Q` and `.`
- Displays whether a solution was found
- Displays the fitness value of the best individual

## How to Compile and Run

### Compile with GCC

Open a terminal in the project folder and run:

```bash
gcc src/n_queens_problem.c -o n_queens_problem
```

### Run on Windows PowerShell

```bash
.\n_queens_problem.exe
```

### Run on macOS or Linux

```bash
./n_queens_problem
```

## Example Program Output

Example for N = 5:

```text
Enter N (4-20): 5

Solution for N = 5:
. Q . . .
. . . Q .
. . . . Q
Q . . . .
. . Q . .

Solution found: YES
Fitness of best individual: 10
```

Example for N = 20:

```text
Enter N (4-20): 20

Solution for N = 20:
. . Q . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . Q . . . . . . .
...

Solution found: YES
Fitness of best individual: 190
```

Because the algorithm uses randomization, the exact board configuration can be different in each run.

## Purpose of the Project

The purpose of this project is to practice solving an optimization problem using a genetic algorithm. It shows how candidate solutions can be represented, evaluated, selected, recombined, mutated, and improved over generations.

## Project Outcome

This project demonstrates the ability to implement a genetic algorithm from scratch in C. It also shows practical use of arrays, pointers, structures, dynamic memory allocation, randomization, modular functions, and algorithmic problem solving.
