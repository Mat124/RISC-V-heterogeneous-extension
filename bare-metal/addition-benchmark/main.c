#include <stdint.h>
#include <stdlib.h>

#include "common.h"
#include "kprintf.h"

#define CLINT_BASE_ADDR 0x02000000
#define MSIP_PER_HART_OFFSET    0x4
#define MSIP_BASE_ADDR(hartid)  (CLINT_BASE_ADDR + (hartid * MSIP_PER_HART_OFFSET))

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

    kprintf("Hart %d: Starting addition benchmark size %d\n", hart, SIZE);
    uint64_t start_cycles, end_cycles;
    asm volatile ("csrr %0, 0xB00" : "=r" (start_cycles));
    for (int i = 0; i < ITERS; i++) {
        addition_iter((uint32_t *) in_a, (uint32_t *) in_b, (uint32_t *) res);
    }
    asm volatile ("csrr %0, 0xB00" : "=r" (end_cycles));

    kprintf("Hart %d: Addition benchmark took %d cycles\n", hart, end_cycles - start_cycles);

    if (hart == 0) { //start other hart
        kprintf("Hart %d: Starting other hart\n", hart);
        usleep(1000000);
        uintptr_t msip_addr = MSIP_BASE_ADDR(1); //MSIP for hart 1
        (*(uint32_t *)(msip_addr)) = 0x1; //create interrupt
    } 

    return 0;
}