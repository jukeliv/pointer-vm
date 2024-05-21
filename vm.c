#include "vm.h"

#define PEEK_RAM(vm, index) *(u16*)(vm->memory + (index))
#define PEEK_ROM(vm, index) *(u16*)(vm->data + (index))

#ifdef POINTER_DEBUG
void vm_dump_memory(vm_t* vm, u16 max_memory_index) {
    for(u16 i = 0; i < max_memory_index; ++i) {
        printf("| 0x%02X | ", *(u8*)(vm->memory+i));
    }
    putchar('\n');
}
#endif

u8 vm_popU8_stack(vm_t* vm) {
    --vm->sp;
    return vm->stack[vm->sp];
}

void vm_pushU8_stack(vm_t* vm, u8 num) {
    vm->stack[vm->sp++] = num;
}

u16 vm_popU16_stack(vm_t* vm) {
    vm->sp -= 2;
    return *(u16*)(vm->stack+vm->sp);
}

void vm_pushU16_stack(vm_t* vm, u16 num) {
    *(u16*)(vm->stack+vm->sp) = num;
    vm->sp += 2;
}


u16 vm_read_u16(vm_t* vm){
    u16 value = PEEK_ROM(vm, vm->ip);
    vm->ip += 2;
    return value;
}

void vm_skip_instruction(vm_t* vm){
    u8 op = vm->data[vm->ip++];

    switch(op) {
        case OpHalt: {} break;

        // mov <constant>, <ptr>
        case OpMove: {
            vm->ip += 2;
            vm->ip += 2;
        } break;
            
        // add <Cptr>, <Aptr>, <Bptr>
        case OpAdd: {
            vm->ip += 2;
            vm->ip += 2;
            vm->ip += 2;
        } break;
            
        // sub <ptr>, <ptr>
        case OpSub: {
            vm->ip += 2;
            vm->ip += 2;
            vm->ip += 2;
        } break;
    
        // lt <Cptr>, <Aptr>, <Bptr>
        case OpLt:{
            vm->ip += 2;
            vm->ip += 2;
            vm->ip += 2;
        } break;
    
        // gt <Cptr>, <Aptr>, <Bptr>
        case OpGt:{
            vm->ip += 2;
            vm->ip += 2;
            vm->ip += 2;
        } break;
    
        // eq <Cptr>, <Aptr>, <Bptr>
        case OpEq:{
            vm->ip += 2;
            vm->ip += 2;
            vm->ip += 2;
        } break;
        
        // peek <ptr2>, <ptr1>
        case OpPeek: {
            vm->ip += 2;
            vm->ip += 2;
        } break;
            
        // if <ptr>
        case OpIf: {
            vm->ip += 2;
        } break;
            
        // jmp <addr>
        case OpJmp: {
            vm->ip += 2;
        } break;
            
        // jmp_in <addr>
        case OpJmpIn: {
            vm->ip += 2;
        } break;
        
        // push <addr>
        case OpPush: {
            vm->ip += 2;
        } break;
        
        // pop <addr>
        case OpPop: {
            vm->ip += 2;
        } break;

        // popb <addr>
        case OpPopB: {
            vm->ip += 2;
        } break;

        // ret
        case OpReturn: {} break;
        
        // call <addr>
        case OpCall: {
            vm->ip += 2;
        } break;

        // sys        
        case OpSyscall: {} break;

        default: todo("Implement!"); break;
    }
}

void execute_vm(vm_t* vm) {
    do {
        u8 op = vm->data[vm->ip++];

        // printf("op = 0x%02X\n", op);
        // printf("ip = 0x%04X\n", vm->ip);
        
        switch(op) {
            case OpHalt: {
                vm->halted = true;
            } break;

            // mov <constant>, <ptr>
            case OpMove: {
                u16 value   = vm_read_u16(vm);
                u16 ptr     = vm_read_u16(vm);

                PEEK_RAM(vm, ptr) = value;
            } break;
            
            // add <Cptr>, <Aptr>, <Bptr>
            case OpAdd: {
                u16 c   = vm_read_u16(vm);
                u16 a   = vm_read_u16(vm);
                u16 b   = vm_read_u16(vm);
                PEEK_RAM(vm, c) = PEEK_RAM(vm, a) + PEEK_RAM(vm, b);
            } break;
            
            // sub <Cptr>, <Aptr>, <Bptr>
            case OpSub: {
                u16 c   = vm_read_u16(vm);
                u16 a   = vm_read_u16(vm);
                u16 b   = vm_read_u16(vm);
                PEEK_RAM(vm, c) = PEEK_RAM(vm, a) - PEEK_RAM(vm, b);
            } break;

            // lt <Cptr>, <Aptr>, <Bptr>
            case OpLt: {
                u16 c   = vm_read_u16(vm);
                u16 a   = vm_read_u16(vm);
                u16 b   = vm_read_u16(vm);
                PEEK_RAM(vm, c) = PEEK_RAM(vm, a) < PEEK_RAM(vm, b);
            } break;

            // gt <Cptr>, <Aptr>, <Bptr>
            case OpGt: {
                u16 c   = vm_read_u16(vm);
                u16 a   = vm_read_u16(vm);
                u16 b   = vm_read_u16(vm);
                PEEK_RAM(vm, c) = PEEK_RAM(vm, a) > PEEK_RAM(vm, b);
            } break;

            // eq <Cptr>, <Aptr>, <Bptr>
            case OpEq: {
                u16 c   = vm_read_u16(vm);
                u16 a   = vm_read_u16(vm);
                u16 b   = vm_read_u16(vm);
                PEEK_RAM(vm, c) = (PEEK_RAM(vm, a) == PEEK_RAM(vm, b));
            } break;
            
            // peek <ptr2>, <ptr1>
            case OpPeek: {
                u16 ptr2    = vm_read_u16(vm);
                u16 ptr1    = vm_read_u16(vm);
                PEEK_RAM(vm, ptr1) = PEEK_RAM(vm, ptr2);
            } break;
            
            // if <ptr>
            case OpIf: {
                u16 ptr = vm_read_u16(vm);
                if(PEEK_RAM(vm, ptr))
                    vm_skip_instruction(vm);
            } break;
            
            // jmp <addr>
            case OpJmp: {
                u16 addr = vm_read_u16(vm);
                vm->ip = addr;
            } break;

            // jmp_in <addr>
            case OpJmpIn:
                vm->ip = PEEK_RAM(vm, vm_read_u16(vm));
            break;

            // ret
            case OpReturn:
                vm->ip = vm->call_stack[--vm->cp];
            break;

            // call <addr>
            case OpCall: {
                u16 addr = vm_read_u16(vm);
                vm->call_stack[vm->cp++] = vm->ip;
                vm->ip = addr;
            } break;
            
            // push <addr>
            case OpPush: {
                u16 addr = vm_read_u16(vm);
                vm_pushU16_stack(vm, PEEK_RAM(vm, addr));
            } break;

            // pop <addr>
            case OpPop: {
                u16 addr = vm_read_u16(vm);
                PEEK_RAM(vm, addr) = vm_popU16_stack(vm);
            } break;
            
            // pushb <addr>
            case OpPushB: {
                u16 addr = vm_read_u16(vm);
                vm_pushU8_stack(vm, PEEK_RAM(vm, addr));
            } break;

            // popb <addr>
            case OpPopB: {
                u16 addr = vm_read_u16(vm);
                PEEK_RAM(vm, addr) = vm_popU8_stack(vm);
            } break;

            // sys
            case OpSyscall: {
                u16 sn = *(u16*)vm->memory;
                switch(sn) {
                    // syscall 0x00 -> print character to stdout
                    case 0x00: {
                        char c = vm_popU8_stack(vm);
                        putchar(c);
                    } break;
                    // syscall 0x01 -> read character from stdin, and push onto the stack
                    case 0x01: {
                        char c = getchar();
                        vm_pushU8_stack(vm, c);
                    } break;
                    // syscall 0x02 -> call outsider function
                    case 0x02: {
                        u8 function_index = vm_popU8_stack(vm);
                        vm->external[function_index](vm);
                    } break;
                }
            }break;

            default:
                todo("Implement!");
            break;
        }
    }while(!vm->halted);
}