#include <stdint.h>
#include <stdlib.h>

#include "common.h"
#include "kprintf.h"


#define CLINT_BASE_ADDR 0x02000000
#define MSIP_PER_HART_OFFSET    0x4
#define MSIP_BASE_ADDR(hartid)  (CLINT_BASE_ADDR + (hartid * MSIP_PER_HART_OFFSET))

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

    kprintf("This is hart %d!\n", hart); //not portable as using %d
    usleep(1000000);

    if (hart == 0) { //start other hart
        uintptr_t msip_addr = MSIP_BASE_ADDR(1); //MSIP for hart 1
        (*(uint32_t *)(msip_addr)) = 0x1; //create interrupt
    }
    
    return 0;
}
