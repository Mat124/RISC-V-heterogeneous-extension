#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define SIZE 40
#define ITERS 100

void addition_iter(uint32_t *in_a, uint32_t *in_b, uint32_t *res) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            res[i] = 0;
            for (int k = 0; k < SIZE; k++) {
                res[i] += in_a[i * SIZE + k] + in_b[k * SIZE + j];
            }
        }
    }
}

int main(void) {
    printf("\n");

    uint32_t in_a[SIZE*SIZE];
    uint32_t in_b[SIZE*SIZE];
    uint32_t res[SIZE*SIZE];

    for (int i = 0; i < SIZE*SIZE; i++) {
        in_a[i] = i;
        in_b[i] = i;
    }

    printf("Hart 0: Starting addition benchmark size %d\n", SIZE);
    uint64_t start_cycles, end_cycles;
    asm volatile ("rdcycle %0" : "=r" (start_cycles));
    for (int i = 0; i < ITERS; i++) {
        addition_iter((uint32_t *) in_a, (uint32_t *) in_b, (uint32_t *) res);
    }
    asm volatile ("rdcycle %0" : "=r" (end_cycles));

    printf("Hart 0: Addition benchmark took %d cycles\n", end_cycles - start_cycles);

    return 0;
    
}