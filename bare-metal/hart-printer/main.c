#include <stdint.h>
#include <stdlib.h>

#include "common.h"
#include "kprintf.h"

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

    for (;;) {
        kprintf("This is hart %d!\n", hart); //not portable as using %d
        usleep(1000000);
    }
    
    return 0;
}
