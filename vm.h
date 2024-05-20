#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef VM_H_
#define VM_H_

#define u8  unsigned char
#define u16 unsigned short

#define todo(msg) printf("todo at line %u in file: %s: %s\n", __LINE__, __FILE__, msg); exit(1)

typedef struct vm_t vm_t;

typedef void(* ExternalFunc)(vm_t*);

// 16 bit machine
struct vm_t{
    u8 data[0xFFFF];
    u8 memory[0xFFFF];
    u8 stack[0xFFFF];
    ExternalFunc external[0xFF];
    u16 sp;         // relative address pointer for the stack
    u16 ip;         // relative address pointer for the instructions
    bool halted;
};

enum operations {
    OpHalt,
    OpMove,
    OpAdd,
    OpSub,
    OpLt,
    OpGt,
    OpEq,
    OpPeek,
    OpIf,
    OpJmp,
    OpJmpIn,
    OpPush,
    OpPop,
    OpSyscall,
    OpPushB,
    OpPopB,
};

#if POINTER_DEBUG
void vm_dump_memory(vm_t* vm, u16 max_memory_index) {
    for(u16 i = 0; i < max_memory_index; ++i) {
        printf("| 0x%02X | ", *(u8*)(vm->memory+i));
    }
    putchar('\n');
}
#endif

u8 vm_popU8_stack(vm_t* vm);
void vm_pushU8_stack(vm_t* vm, u8 num);
u16 vm_popU16_stack(vm_t* vm);
void vm_pushU16_stack(vm_t* vm, u16 num);

u16 vm_read_u16(vm_t* vm);

void vm_skip_instruction(vm_t* vm);

void execute_vm(vm_t* vm);

#endif // VM_H_