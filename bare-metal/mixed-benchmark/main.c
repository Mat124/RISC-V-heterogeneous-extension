#include <stdint.h>
#include <stdlib.h>

#include "common.h"
#include "kprintf.h"

#define SIZE 40
#define ITERS 100

void mixed_bench(uint32_t *in_a, uint32_t *in_b, uint32_t *res, uint32_t sel) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            sel = 1 - sel;
            res[i] = 0;
            if (sel) {
                for (int k = 0; k < SIZE; k++) {
                    res[i] += in_a[i * SIZE + k] * in_b[k * SIZE + j];
                }
            }
            else {
                for (int k = 0; k < SIZE; k++) {
                    res[i] += in_a[i * SIZE + k] + in_b[k * SIZE + j];
                }
            }
        }
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
    uint32_t sel;

    for (int i = 0; i < SIZE; i++) {
        in_a[i] = i;
        in_b[i] = i;
    }

    sel = 0;

    kprintf("Hart %d: Starting mixed benchmark size %d\n", hart, SIZE);
    uint64_t start_cycles, end_cycles;
    asm volatile ("csrr %0, 0xB00" : "=r" (start_cycles));
    for (int i = 0; i < ITERS; i++) {
        sel = 1 - sel;
        mixed_bench((uint32_t *) in_a, (uint32_t *) in_b, (uint32_t *) res, sel);
    }
    asm volatile ("csrr %0, 0xB00" : "=r" (end_cycles));

    kprintf("Hart %d: Mixed benchmark took %d cycles\n", hart, end_cycles - start_cycles);

    return 0;
}