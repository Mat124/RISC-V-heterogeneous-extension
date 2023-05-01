# RISC-V bare-metal programs

The bootrom loads and executes boot.elf file from SD card DOS partition.
boot.elf is regular executable ELF, it can contain any software suitable for RISC-V RV64 M mode.

## "Hello World" program

Runs on any RISC-V 64-bit configuration.
Outputs "Hello World!" message using UART.

Usage:
```
cd bare-metal/hello-world
make clean all
```
Copy boot.elf to the SD card.

## Using Xilinx debugger

Run RISC-V core with debug module instantiated. \
Load Xilinx debugger and target the core to load program to. \
Use `load` command to start programming running on core.