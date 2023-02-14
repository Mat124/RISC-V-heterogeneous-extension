#include <stdint.h>
#include <stdlib.h>

#include "common.h"
#include "kprintf.h"

#define SIZE 10000
#define ITERS 100

void io_iter(uint32_t *in_a, uint32_t *in_b, uint32_t *res) {
    for (int i = 0; i < SIZE; i++) {
        res[i] = 0;
        res[i] = in_a[i];
        res[i] = in_b[i];
        res[i] = in_a[SIZE-i-1];
        res[i] = in_b[SIZE-i-1]; //diff ends of the array for cache misses (hopefully)
    }
}

static void usleep(unsigned us) {
    uintptr_t cycles0;
    uintptr_t cycles1;
    asm volatile ("csrr %0, 0xB00" : "=r" (cycles0));
    for (;;) {
        asm volatile ("csrr %0, 0xB00" : "=r" (cycles1));
        if (cycles1 - cycles0 >= us * 100) break;
    }
}

int main(void) {
    kprintf("\n");
    uint32_t hart;
    asm volatile (
        "csrr %0, mhartid" : "=r" (hart)
    );

    uint32_t in_a[SIZE];
    uint32_t in_b[SIZE];
    uint32_t res[SIZE];

    for (int i = 0; i < SIZE; i++) {
        in_a[i] = i;
        in_b[i] = i;
    }
    
    kprintf("Hart %d: Starting IO benchmark size %d\n", hart, SIZE);
    uint64_t start_cycles, end_cycles;
    asm volatile ("csrr %0, 0xB00" : "=r" (start_cycles));
    for (int i = 0; i < ITERS; i++) {
        io_iter((uint32_t *) in_a, (uint32_t *) in_b, (uint32_t *) res);
    }
    asm volatile ("csrr %0, 0xB00" : "=r" (end_cycles));

    kprintf("Hart %d: IO benchmark took %d cycles\n", hart, end_cycles - start_cycles);

    return 0;
}