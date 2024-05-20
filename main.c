#include <stdio.h>

#define POINTER_DEBUG 0
#include "vm.h"

void addition(vm_t* vm) {
    vm_pushU16_stack(vm, vm_popU16_stack(vm)+vm_popU16_stack(vm));
}

void print_u16(vm_t* vm) {
    u16 v = vm_popU16_stack(vm);
    printf("%d\n", v);
}

int main(int argc, char** argv) {
    if(argc < 2) {
        return 1;
    }

    FILE* fp = fopen(argv[1], "rb");

    vm_t vm = {0};

    fread(&vm, sizeof(vm), 1, fp) != sizeof(vm);

    fclose(fp);

    vm.external[0] = addition;
    vm.external[1] = print_u16;

    execute_vm(&vm);

    //vm_dump_memory(&vm, 2);
    return 0;
}