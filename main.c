#include <stdio.h>

#define ARR_SIZE(arr) (sizeof(arr)/sizeof(*arr))
#include "vm.h"

int main(int argc, char** argv) {
    if(argc < 2) {
        return 1;
    }

    FILE* fp = fopen(argv[1], "rb");

    vm_t vm = {0};

    fread(vm.data, sizeof(*vm.data), ARR_SIZE(vm.data), fp) != sizeof(vm.data);

    fclose(fp);

    execute_vm(&vm);

    //vm_dump_memory(&vm, 2);
    return 0;
}