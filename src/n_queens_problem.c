/*
  n_queens_problem.c
  Project #3: Solving the N-Queens Problem using a Genetic Algorithm

  This program solves the N-Queens problem by using a genetic algorithm approach.
  Each solution is represented as a permutation where each column contains exactly one queen, and the value stored represents the row of that queen.
  The program outputs only the required results:
  - Whether a valid solution is found
  - The final board layout displayed with '.' and 'Q'
  - The fitness value of the best solution
  The fitness of a solution is calculated by counting diagonal conflicts.
  The permutation representation already prevents row and column conflicts, so only diagonal attacks are considered.
  A solution is considered perfect when there are no diagonal conflicts.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Structure that represents one solution candidate in the genetic algorithm
typedef struct {
    int *genes;     // genes stores the board configuration as an array
                    // each index represents a column on the chessboard
                    // the value at that index gives the row where the queen is placed
    int fitness;    // fitness shows how good this solution is
                    // it is computed from the number of diagonal conflicts
                    // higher fitness means fewer conflicts and a better solution
} Individual;

//Generates a random integer between the given minimum and maximum values 
static int rand_int(int min, int max) {
    // Uses rand() and scales its output so the result stays within the required range
    return min + rand() % (max - min + 1);
}

// Swaps the values of two integers, mainly used during mutation and shuffling 
static void swap_int(int *a, int *b) {
    int t = *a;   // stores the value at address a temporarily
    *a = *b;      // assigns the value at address b to address a
    *b = t;       // puts the original value of a into address b
}

// Calculates how many diagonal conflicts exist in the given board representation
static int count_diagonal_conflicts(const int *genes, int N) {
    int conflicts = 0;   // counts the number of attacking queen pairs on diagonals

    for (int c1 = 0; c1 < N; c1++) {
        for (int c2 = c1 + 1; c2 < N; c2++) {
            int r1 = genes[c1];   // row position of queen in column c1
            int r2 = genes[c2];   // row position of queen in column c2

            int row_diff = r1 - r2;   // shows the difference between row positions
            if (row_diff < 0) row_diff = -row_diff;  // converts to absolute value

            int col_diff = c2 - c1;   // shows the difference between column positions

            // Queens are on the same diagonal if row difference equals column difference
            if (row_diff == col_diff) {
                conflicts++;          // counts this diagonal conflict
            }
        }
    }
    return conflicts;                // returns total number of diagonal conflicts
}

// Calculates the maximum possible number of non attacking queen pairs for a board of size N
static int max_pairs(int N) {
    // This formula counts how many unique pairs can be formed from N queens
    // It represents the highest possible fitness value when there are no diagonal conflicts
    return (N * (N - 1)) / 2;
}

// Computes and assigns the fitness value for a single individual
static void evaluate_fitness(Individual *ind, int N) {
    // First, count how many diagonal conflicts exist in the current board
    int conflicts = count_diagonal_conflicts(ind->genes, N);

    // Fitness is calculated by subtracting conflicts from the maximum possible pairs
    // Fewer conflicts result in a higher fitness score
    ind->fitness = max_pairs(N) - conflicts;
}

// Randomly rearranges the elements of the array to create a random permutation
static void shuffle(int *array, int N) {
    // Starts from the last index and moves backwards through the array
    for (int i = N - 1; i > 0; i--) {
        // Selects a random index between 0 and the current index i
        int j = rand_int(0, i);
        // Swaps the elements at positions i and j to randomize the order
        swap_int(&array[i], &array[j]);
    }
}

// Creates the initial population of candidate solutions for the genetic algorithm
static void initialize_population(Individual *pop, int pop_size, int N) {
    // Loops over each individual in the population
    for (int i = 0; i < pop_size; i++) {
        // Allocates memory for the genes array of the current individual
        pop[i].genes = (int *)malloc(N * sizeof(int));
        // Checks if memory allocation was successful
        if (!pop[i].genes) {
            printf("Memory allocation failed.\n");
            exit(1);
        }

        // Initializes the genes as a basic permutation where each column has one unique row
        for (int j = 0; j < N; j++) {
            pop[i].genes[j] = j;
        }

        // Randomizes the permutation so each individual starts with a different board
        shuffle(pop[i].genes, N);
        // Calculates and assigns the fitness value of this individual
        evaluate_fitness(&pop[i], N);
    }
}

// Selects one parent from the population using tournament selection
static int tournament_selection(const Individual *pop, int pop_size, int k) {
    int best = rand_int(0, pop_size - 1);   // randomly chooses an initial individual as the current best

    // repeats the tournament k times to compare multiple individuals
    for (int i = 1; i < k; i++) {
        int challenger = rand_int(0, pop_size - 1);   // randomly picks another individual from the population

        // if the challenger has higher fitness, it becomes the new best
        if (pop[challenger].fitness > pop[best].fitness) {
            best = challenger;
        }
    }

    return best;   // returns the index of the selected parent
}

// Performs crossover between two parent solutions while keeping a valid permutation
static void crossover_prefix_fill(const int *parent1, const int *parent2, int *child, int N) {
    int cut = rand_int(1, N - 2);   // selects a random cut point to split the parents

    int *used = (int *)calloc(N, sizeof(int));   // keeps track of which rows are already used in the child
    if (!used) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    // copies the first part of parent1 into the child up to the cut point
    for (int i = 0; i <= cut; i++) {
        child[i] = parent1[i];      // copies the gene from parent1
        used[child[i]] = 1;         // marks this row as already placed
    }

    int idx = cut + 1;              // starting index for filling the remaining positions
    for (int i = 0; i < N; i++) {
        int val = parent2[i];       // checks genes from parent2 in order
        if (!used[val]) {           // only adds the gene if it is not already used
            child[idx++] = val;     // places the gene into the next empty position
            used[val] = 1;          // marks the row as used
            if (idx == N) break;    // stops when the child array is completely filled
        }
    }

    free(used);                     // releases temporary memory
}

// Performs a mutation by swapping the positions of two randomly chosen queens
static void mutate_swap(int *genes, int N) {
    int i = rand_int(0, N - 1);   // randomly chooses the first column index
    int j = rand_int(0, N - 1);   // randomly chooses the second column index
    swap_int(&genes[i], &genes[j]); // swaps the row positions of the two selected columns
}

// Finds the index of the individual with the highest fitness in the population
static int best_individual_index(const Individual *pop, int pop_size) {
    int best = 0;                // assumes the first individual is the best at the start
    for (int i = 1; i < pop_size; i++) {
        // compares fitness values and update best when a better solution is found
        if (pop[i].fitness > pop[best].fitness)
            best = i;
    }
    return best;                 // returns the index of the best individual
}

// Runs the genetic algorithm to try to solve the N-Queens problem
// Returns 1 if a perfect solution is found, otherwise returns 0
static int run_ga(int N, int pop_size, int max_gen, double mutation_rate, Individual *best) {

    // Allocate memory for the current population and the next generation
    Individual *population = (Individual *)malloc(pop_size * sizeof(Individual));
    Individual *new_population = (Individual *)malloc(pop_size * sizeof(Individual));

    // Checks if memory allocation was successful
    if (!population || !new_population) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    // Initializes the population with random valid permutations
    initialize_population(population, pop_size, N);

    // Allocates memory for each individual in the new population
    for (int i = 0; i < pop_size; i++) {
        new_population[i].genes = (int *)malloc(N * sizeof(int));
        new_population[i].fitness = 0;
    }

    int maxFitness = max_pairs(N);   // stores the maximum possible fitness value
    int found = 0;                   // shows whether a perfect solution is found

    // Main loop that runs for a fixed number of generations
    for (int gen = 0; gen < max_gen; gen++) {

        // Finds the index of the best individual in the current population
        int bi = best_individual_index(population, pop_size);

        // Checks if the best individual is a perfect solution
        if (population[bi].fitness == maxFitness) {
            found = 1;
        }

        // Copies the best individual directly to the next generation (elitism)
        for (int j = 0; j < N; j++) {
            new_population[0].genes[j] = population[bi].genes[j];
        }
        new_population[0].fitness = population[bi].fitness;

        // Stops the algorithm early if a perfect solution is found
        if (found) break;

        // Generates the rest of the new population
        for (int i = 1; i < pop_size; i++) {

            // Selects two parents using tournament selection
            int p1 = tournament_selection(population, pop_size, 3);
            int p2 = tournament_selection(population, pop_size, 3);

            // Ensures that the same parent is not selected twice
            while (p2 == p1) {
                p2 = tournament_selection(population, pop_size, 3);
            }

            // Applies crossover to produce a child from the selected parents
            crossover_prefix_fill(population[p1].genes, population[p2].genes, new_population[i].genes, N);

            // Applies mutation with a given probability
            if ((double)rand() / RAND_MAX < mutation_rate) {
                mutate_swap(new_population[i].genes, N);
            }

            // Evaluates the fitness of the new individual
            evaluate_fitness(&new_population[i], N);
        }

        // Swaps the current population with the new population
        Individual *tmp = population;
        population = new_population;
        new_population = tmp;
    }

    // Finds the best individual after the algorithm finishes
    int bi = best_individual_index(population, pop_size);

    // Allocates memory to store the best solution
    best->genes = (int *)malloc(N * sizeof(int));

    // Copies the genes of the best individual
    for (int j = 0; j < N; j++) {
        best->genes[j] = population[bi].genes[j];
    }
    best->fitness = population[bi].fitness;   // stores the fitness of the best solution

    // Frees all dynamically allocated memory for populations
    for (int i = 0; i < pop_size; i++) {
        free(population[i].genes);
        free(new_population[i].genes);
    }
    free(population);
    free(new_population);

    // Returns whether a perfect solution was found
    return found;
}

// Prints the N x N board in the required . and Q format using the genes array
static void print_board(const int *genes, int N) {
    for (int r = 0; r < N; r++) {                      // goes through each row from top to bottom
        for (int c = 0; c < N; c++) {                  // goes through each column in the current row
            printf(genes[c] == r ? "Q" : ".");         // prints Q if the queen in column c is placed on this row, otherwise prints .
            if (c < N - 1) printf(" ");                // prints spaces between symbols so the output matches the sample format
        }
        printf("\n");                                  // moves to the next row after finishing one row
    }
}

// Runs the program: reads N, runs the genetic algorithm, then prints the final result in the required format
int main(void) {
    srand((unsigned int)time(NULL));                   // sets a different random seed each run so the GA does not start the same every time

    int N;
    printf("Enter N (4-20): ");                        // asks the user for the board size
    if (scanf("%d", &N) != 1 || N < 4 || N > 20) {     // checks that input is a number and also inside the allowed range
        printf("Invalid N.\n");                        // prints an error message if input is invalid
        return 1;                                      // ends the program with an error code
    }

    int pop_size = 200;                                // sets how many individuals exist in the population each generation
    int max_gen = 2000;                                // sets the maximum number of generations to try before stopping
    double mutation_rate = 0.05;                       // sets the chance of applying mutation after crossover

    Individual best;
    best.genes = NULL;                                 // starts as NULL so we can safely check before freeing later
    best.fitness = 0;                                  // starts at 0, run_ga overwrites it with the best fitness found

    int found = run_ga(N, pop_size, max_gen, mutation_rate, &best);  // runs GA and stores the best solution inside best

    printf("\nSolution for N = %d:\n", N);             // prints the required header line
    print_board(best.genes, N);                        // prints the board using the best solution that the GA found

    printf("\nSolution found: %s\n", found ? "YES" : "NO");          // prints YES if a perfect solution is found, otherwise NO
    printf("Fitness of best individual: %d\n", best.fitness);        // prints the fitness score of the best solution

    if (best.genes) {                                  // checks that best.genes was actually allocated before freeing it
        free(best.genes);                              // releases the memory allocated for the best solution array
        best.genes = NULL;                             // sets it to NULL to avoid a dangling pointer after free
    }

    return 0;                                          // ends the program 
}
