:: This file is made only for me @jukeliv to build and test fast
:: It may or not work on your machine ( even tho it's just like 2 gcc commands but, still )
@echo off
gcc ./src/vm.c ./src/main.c -o ./build/ptr -I./include/
gcc ./src/assembler.c -o ./build/asm2ptr -I./include/