#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define MAX_SIZE 1024

// Function to multiply matrices using different algorithms
void matrix_multiply(int n, int A[MAX_SIZE][MAX_SIZE], int B[MAX_SIZE][MAX_SIZE], int C[MAX_SIZE][MAX_SIZE], int variant) {
    int i, j, k;
    switch (variant) {
        case 0: // ijk
            for (i = 0; i < n; i++) {
                for (j = 0; j < n; j++) {
                    C[i][j] = 0;
                    for (k = 0; k < n; k++) {
                        C[i][j] += A[i][k] * B[k][j];
                    }
                }
            }
            break;
        case 1: // ikj
            for (i = 0; i < n; i++) {
                for (k = 0; k < n; k++) {
                    for (j = 0; j < n; j++) {
                        C[i][j] += A[i][k] * B[k][j];
                    }
                }
            }
            break;
        case 2: // kij
            for (k = 0; k < n; k++) {
                for (i = 0; i < n; i++) {
                    for (j = 0; j < n; j++) {
                        C[i][j] += A[i][k] * B[k][j];
                    }
                }
            }
            break;
        case 3: // jik
            for (j = 0; j < n; j++) {
                for (i = 0; i < n; i++) {
                    C[i][j] = 0;
                    for (k = 0; k < n; k++) {
                        C[i][j] += A[i][k] * B[k][j];
                    }
                }
            }
            break;
        case 4: // jki
            for (j = 0; j < n; j++) {
                for (k = 0; k < n; k++) {
                    for (i = 0; i < n; i++) {
                        C[i][j] += A[i][k] * B[k][j];
                    }
                }
            }
            break;
        case 5: // kji
            for (k = 0; k < n; k++) {
                for (j = 0; j < n; j++) {
                    for (i = 0; i < n; i++) {
                        C[i][j] += A[i][k] * B[k][j];
                    }
                }
            }
            break;
    }
}

// Function to perform matrix multiplication with OpenMP
void matrix_multiply_openmp(int n, int A[MAX_SIZE][MAX_SIZE], int B[MAX_SIZE][MAX_SIZE], int C[MAX_SIZE][MAX_SIZE], int variant, int chunk_size, int dynamic) {
    int i, j, k;
    switch (variant) {
        case 0: // ijk
            #pragma omp parallel for schedule(dynamic, chunk_size) collapse(2)
            for (i = 0; i < n; i++) {
                for (j = 0; j < n; j++) {
                    C[i][j] = 0;
                    for (k = 0; k < n; k++) {
                        C[i][j] += A[i][k] * B[k][j];
                    }
                }
            }
            break;
        // Add more cases for other variants here with similar OpenMP constructs
    }
}

int main() {
    FILE *fp = fopen("results.txt", "w");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    int A[MAX_SIZE][MAX_SIZE], B[MAX_SIZE][MAX_SIZE], C[MAX_SIZE][MAX_SIZE];
    double sequential_time, parallel_time, speedup;

    for (int n = 32; n <= 1024; n *= 2) {
        // Initialize matrices A and B
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                A[i][j] = rand() % 10; // Random values between 0-9
                B[i][j] = rand() % 10; // Random values between 0-9
                C[i][j] = 0;           // Initialize result matrix
            }
        }

        // Sequential execution
        for (int variant = 0; variant < 6; variant++) {
            clock_t start = clock();
            matrix_multiply(n, A, B, C, variant);
            sequential_time = (double)(clock() - start) / CLOCKS_PER_SEC;

            // Parallel execution with OpenMP
            for (int chunk_size = 1; chunk_size <= 10; chunk_size++) {
                clock_t start_parallel = clock();
                matrix_multiply_openmp(n, A, B, C, variant, chunk_size, 1); // 1 for dynamic scheduling
                parallel_time = (double)(clock() - start_parallel) / CLOCKS_PER_SEC;

                // Calculate speedup
                speedup = sequential_time / parallel_time;
                fprintf(fp, "Size: %d, Variant: %d, Chunk Size: %d, Sequential Time: %f, Parallel Time: %f, Speedup: %f\n", 
                        n, variant, chunk_size, sequential_time, parallel_time, speedup);
            }
        }
    }

    fclose(fp);
    return 0;
}
