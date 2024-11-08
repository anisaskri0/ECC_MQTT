#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

int NRA, NCA, NCB;

// Timing routine
double omp_get_wtime();

// Matrix multiply routines
void matrix_multiply(double **a, double **b, double **c);
void matrix_multiply_cache_friendly(double **a, double **b, double **c);
void matrix_multiply_parallel(double **a, double **b, double **c);
void matrix_multiply_cache_friendly_parallel(double **a, double **b, double **c);
void matrix_multiply_parallel_nested(double **a, double **b, double **c);
void matrix_multiply_cache_friendly_parallel_nested(double **a, double **b, double **c);

// Function to initialize matrices and perform matrix multiplication
void initMatrix(double **a, double **b, double **c);
void freeMatrix(double **a, double **b, double **c);

int main() {
    printf("Enter an NRA: ");
    scanf("%d", &NRA);
    printf("Enter an NCA: ");
    scanf("%d", &NCA);
    printf("Enter an NCB: ");
    scanf("%d", &NCB);

    double **a, **b, **c;
    a = (double **)malloc(NRA * sizeof(double *));
    b = (double **)malloc(NCA * sizeof(double *));
    c = (double **)malloc(NRA * sizeof(double *));
    for (int i = 0; i < NRA; i++) {
        a[i] = (double *)malloc(NCA * sizeof(double));
        c[i] = (double *)malloc(NCB * sizeof(double));
    }
    for (int i = 0; i < NCA; i++) {
        b[i] = (double *)malloc(NCB * sizeof(double));
    }

    initMatrix(a, b, c);

    int thread_counts[] = {2, 4, 8}; // Thread counts to test
    double time_results[6][3]; // Store results for 6 types of multiplications
    double speedup_results[6][3]; // Store speedup results for 6 types of multiplications

    for (int t = 0; t < 3; t++) { // Loop through thread counts
        omp_set_num_threads(thread_counts[t]);

        // Non-cache-friendly sequential
        double starttime = omp_get_wtime();
        matrix_multiply(a, b, c);
        time_results[0][t] = omp_get_wtime() - starttime;

        // Cache-friendly sequential
        starttime = omp_get_wtime();
        matrix_multiply_cache_friendly(a, b, c);
        time_results[1][t] = omp_get_wtime() - starttime;

        // Non-cache-friendly parallel
        starttime = omp_get_wtime();
        matrix_multiply_parallel(a, b, c);
        time_results[2][t] = omp_get_wtime() - starttime;

        // Cache-friendly parallel
        starttime = omp_get_wtime();
        matrix_multiply_cache_friendly_parallel(a, b, c);
        time_results[3][t] = omp_get_wtime() - starttime;

        // Non-cache-friendly parallel nested
        starttime = omp_get_wtime();
        matrix_multiply_parallel_nested(a, b, c);
        time_results[4][t] = omp_get_wtime() - starttime;

        // Cache-friendly parallel nested
        starttime = omp_get_wtime();
        matrix_multiply_cache_friendly_parallel_nested(a, b, c);
        time_results[5][t] = omp_get_wtime() - starttime;
    }

    // Calculate speedup
    for (int t = 0; t < 3; t++) {
        speedup_results[0][t] = time_results[0][t] / time_results[0][0]; // Non-cache-friendly sequential speedup
        speedup_results[1][t] = time_results[0][t] / time_results[1][t]; // Cache-friendly sequential speedup
        speedup_results[2][t] = time_results[0][t] / time_results[2][t]; // Non-cache-friendly parallel speedup
        speedup_results[3][t] = time_results[0][t] / time_results[3][t]; // Cache-friendly parallel speedup
        speedup_results[4][t] = time_results[0][t] / time_results[4][t]; // Non-cache-friendly parallel nested speedup
        speedup_results[5][t] = time_results[0][t] / time_results[5][t]; // Cache-friendly parallel nested speedup
    }

    // Print results for each thread count
    for (int t = 0; t < 3; t++) {
        printf("Threads: %d\n", thread_counts[t]);
        printf("Non-cache-friendly sequential: %3.8f s\n", time_results[0][t]);
        printf("Cache-friendly sequential: %3.8f s\n", time_results[1][t]);
        printf("Non-cache-friendly parallel: %3.8f s\n", time_results[2][t]);
        printf("Cache-friendly parallel: %3.8f s\n", time_results[3][t]);
        printf("Non-cache-friendly parallel nested: %3.8f s\n", time_results[4][t]);
        printf("Cache-friendly parallel nested: %3.8f s\n", time_results[5][t]);
    }

    // Save results to a CSV file
    FILE *file = fopen("speedup_results.csv", "w");
    fprintf(file, "Threads,Non-cache-friendly Seq,Cache-friendly Seq,Non-cache-friendly Parallel,Cache-friendly Parallel,Non-cache-friendly Nested,Cache-friendly Nested\n");
    for (int t = 0; t < 3; t++) {
        fprintf(file, "%d,%f,%f,%f,%f,%f,%f\n", thread_counts[t],
                speedup_results[0][t], speedup_results[1][t],
                speedup_results[2][t], speedup_results[3][t],
                speedup_results[4][t], speedup_results[5][t]);
    }
    fclose(file);

    freeMatrix(a, b, c);
    return 0;
}

// Timing routine implementation
double omp_get_wtime() {
    struct timeval end;
    gettimeofday(&end, NULL);
    return end.tv_sec + end.tv_usec * 1.0e-6;
}

// Function to initialize matrices
void initMatrix(double **a, double **b, double **c) {
    for (int i = 0; i < NRA; i++) {
        for (int j = 0; j < NCA; j++) a[i][j] = (double)(i + j);
        for (int j = 0; j < NCB; j++) c[i][j] = 0.0;
    }
    for (int i = 0; i < NCA; i++)
        for (int j = 0; j < NCB; j++)
            b[i][j] = (double)(i * j);
}

// Function to free matrices
void freeMatrix(double **a, double **b, double **c) {
    for (int i = 0; i < NRA; i++) {
        free(a[i]);
        free(c[i]);
    }
    for (int i = 0; i < NCA; i++) {
        free(b[i]);
    }
    free(a);
    free(b);
    free(c);
}

// Non-cache-friendly sequential matrix multiplication
void matrix_multiply(double **a, double **b, double **c) {
    for (int i = 0; i < NRA; i++) {
        for (int j = 0; j < NCB; j++) {
            for (int k = 0; k < NCA; k++) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

// Cache-friendly sequential matrix multiplication (loop order adjusted)
void matrix_multiply_cache_friendly(double **a, double **b, double **c) {
    for (int i = 0; i < NRA; i++) {
        for (int k = 0; k < NCA; k++) {
            for (int j = 0; j < NCB; j++) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

// Non-cache-friendly parallel matrix multiplication
void matrix_multiply_parallel(double **a, double **b, double **c) {
    #pragma omp parallel for
    for (int i = 0; i < NRA; i++) {
        for (int j = 0; j < NCB; j++) {
            for (int k = 0; k < NCA; k++) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

// Cache-friendly parallel matrix multiplication
void matrix_multiply_cache_friendly_parallel(double **a, double **b, double **c) {
    #pragma omp parallel for
    for (int i = 0; i < NRA; i++) {
        for (int k = 0; k < NCA; k++) {
            for (int j = 0; j < NCB; j++) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

// Non-cache-friendly parallel nested matrix multiplication
void matrix_multiply_parallel_nested(double **a, double **b, double **c) {
    #pragma omp parallel
    {
        #pragma omp for
        for (int i = 0; i < NRA; i++) {
            for (int j = 0; j < NCB; j++) {
                for (int k = 0; k < NCA; k++) {
                    c[i][j] += a[i][k] * b[k][j];
                }
            }
        }
    }
}

// Cache-friendly parallel nested matrix multiplication
void matrix_multiply_cache_friendly_parallel_nested(double **a, double **b, double **c) {
    #pragma omp parallel
    {
        #pragma omp for
        for (int i = 0; i < NRA; i++) {
            for (int k = 0; k < NCA; k++) {
                for (int j = 0; j < NCB; j++) {
                    c[i][j] += a[i][k] * b[k][j];
                }
            }
        }
    }
}
