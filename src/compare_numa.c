
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define TOTAL 100000000L
#define MEMORY_MB 512

void run_false_sharing(int threads, int padded)
{
    int stride = padded ? 8 : 1;

    long *counter = NULL;

    if (posix_memalign((void **)&counter, 64,
                       (size_t)threads * stride * sizeof(long)) != 0) {
        printf("Memory allocation failed\n");
        return;
    }

    for (int i = 0; i < threads * stride; i++)
        counter[i] = 0;

    double start = omp_get_wtime();

    #pragma omp parallel num_threads(threads)
    {
        int id = omp_get_thread_num();
        long *value = &counter[id * stride];

        long iterations = TOTAL / threads;

        for (long i = 0; i < iterations; i++)
            (*value)++;
    }

    double elapsed = omp_get_wtime() - start;

    long final_count = 0;

    for (int i = 0; i < threads; i++)
        final_count += counter[i * stride];

    printf("%-10s | Threads=%d | Final Count=%ld | Time=%.4f s | %s\n",
           padded ? "Padded" : "Adjacent",
           threads,
           final_count,
           elapsed,
           final_count == TOTAL ? "Correct" : "Incorrect");

    free(counter);
}

void run_memory_sweep(int threads)
{
    size_t bytes = (size_t)MEMORY_MB * 1024 * 1024;

    unsigned char *buffer = malloc(bytes);

    if (buffer == NULL) {
        printf("Memory allocation failed\n");
        return;
    }

    for (size_t i = 0; i < bytes; i++)
        buffer[i] = 1;

    long long sum = 0;

    double start = omp_get_wtime();

    for (int pass = 0; pass < 4; pass++) {

        #pragma omp parallel for num_threads(threads) reduction(+:sum)
        for (size_t i = 0; i < bytes; i += 64)
            sum += buffer[i];
    }

    double elapsed = omp_get_wtime() - start;

    printf("Memory Sweep | Threads=%d | Buffer=%d MB | Sum=%lld | Time=%.4f s\n",
           threads, MEMORY_MB, sum, elapsed);

    free(buffer);
}

int main(void)
{
    printf("\n============================================\n");
    printf(" EXPERIMENT 4: FALSE SHARING AND NUMA TEST\n");
    printf("============================================\n");

    printf("\nPART 1: FALSE SHARING COMPARISON\n");
    printf("--------------------------------------------\n");

    int thread_counts[] = {1, 2, 4};

    for (int i = 0; i < 3; i++) {
        int threads = thread_counts[i];

        run_false_sharing(threads, 0);
        run_false_sharing(threads, 1);

        printf("\n");
    }

    printf("\nPART 2: NUMA MEMORY ACCESS SWEEP\n");
    printf("--------------------------------------------\n");

    run_memory_sweep(1);
    run_memory_sweep(2);

    printf("\n============================================\n");
    printf(" COMPARISON COMPLETED\n");
    printf("============================================\n");

    return 0;
}
