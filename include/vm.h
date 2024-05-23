#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef VM_H_
#define VM_H_

#define POINTER_DEBUG

#define u8  unsigned char
#define u16 unsigned short

#define todo(msg) \
    do { \
        printf("todo at line %u in file: %s: %s\n", __LINE__, __FILE__, msg); \
        exit(1); \
    } while(0)

typedef struct vm_t vm_t;

typedef void(* ExternalFunc)(vm_t*);

// 16 bit machine
struct vm_t{
    /*
        ROM layout:
        0x00 to 0x400 -> the stack

    */
    u8 data[0xFFFF];
    u8 memory[0xFFFF];
    ExternalFunc external[0xFF];
    u16 r[3];       // general registers ( r0, r1, r2 )
    u16 sp;         // stack pointer
    u16 bp;         // base pointer
    u16 ip;         // relative address pointer for the instructions
    bool halted;
};

enum operations {
    OpHlt,
    
    OpMoveCA, // move constant to address
    OpMoveCR, // move constant to register
    OpMoveAR, // move address to address
    OpMoveRR, // move register to register

    OpAddAC,  // r0 = *addr  + const
    OpAddAA,  // r0 = *addr1 + *addr2
    OpAddRC,  // r0 =  reg   + const

    OpEqAA,   // r0 = *addr1 == *addr2

    OpPeek,
    OpIf,
    OpJmp,
    OpJmpIn,
    
    OpPushReg,
    OpPushAddr,
    
    OpPopReg,
    OpPopAddr,
    
    OpPushAddrB,
    OpPopAddrB,

    OpSyscall,
    
    OpReturn,
    OpCall,
    OpLeave,
};

#ifdef POINTER_DEBUG
void vm_dump_memory(vm_t* vm, u16 max_memory_index);
#endif

u16* vm_get_register(vm_t* vm, u8 index);

u8 vm_popU8_stack(vm_t* vm);
void vm_pushU8_stack(vm_t* vm, u8 num);
u16 vm_popU16_stack(vm_t* vm);
void vm_pushU16_stack(vm_t* vm, u16 num);

u16 vm_read_u16(vm_t* vm);

void vm_skip_instruction(vm_t* vm);

void execute_vm(vm_t* vm);

#endif // VM_H_