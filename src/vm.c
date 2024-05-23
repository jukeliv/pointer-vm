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

u16* vm_get_register(vm_t* vm, u8 index) {
    switch(index) {
        // r0 to r2
        case 0x00:
            return &vm->r[index];
        break;
        case 0x01:
            return &vm->r[index];
        break;
        case 0x02:
            return &vm->r[index];
        break;
        // rsp
        case 0x03:
            return &vm->sp;
        break;
        // rbp
        case 0x04:
            return &vm->bp;
        break;

        default:
            printf("index = 0x%02X\n", index);
            todo("Add more registers!");
        break;
    }
}

u8 vm_popU8_stack(vm_t* vm) {
    --vm->sp;
    return vm->memory[vm->sp];
}

void vm_pushU8_stack(vm_t* vm, u8 num) {
    vm->memory[vm->sp++] = num;
}

u16 vm_popU16_stack(vm_t* vm) {
    vm->sp -= 2;
    return *(u16*)(vm->memory+vm->sp);
}

void vm_pushU16_stack(vm_t* vm, u16 num) {
    *(u16*)(vm->memory+vm->sp) = num;
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
        case OpHlt: {} break;

        // mov <constant>, <ptr>
        case OpMoveCA: {
            vm->ip += 2;
            vm->ip += 2;
        } break;
        // mov <ptr>, <register>
        case OpMoveAR: {
            vm->ip += 2;
            vm->ip += 2;
        } break;
        // mov <constant>, <register>
        case OpMoveCR: {
            vm->ip += 2;
            vm->ip += 2;
        } break;
        // mov <register>, <register>
        case OpMoveRR: {
            vm->ip += 2;
            vm->ip += 2;
        } break;
            
        // add <Aptr>, <Bptr>
        case OpAddAA: {
            vm->ip += 2;
            vm->ip += 2;
        } break;
        
        // add <ptr>, <const>
        case OpAddAC: {
            vm->ip += 2;
            vm->ip += 2;
        } break;
        
        // add <register>, <const>
        case OpAddRC: {
            vm->ip += 2;
            vm->ip += 2;
        } break;
        
        // add <Aptr>, <Bptr>
        case OpEqAA: {
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
        case OpPushAddr: {
            vm->ip += 2;
        } break;

        // push <register>
        case OpPushReg: {
            ++vm->ip;
        } break;

        // pop <addr>
        case OpPopAddr: {
            vm->ip += 2;
        } break;

        // pop <register>
        case OpPopReg: {
            ++vm->ip;
        } break;

        
        // pushb <addr>
        case OpPushAddrB: {
            vm->ip += 2;
        } break;

        // popb <addr>
        case OpPopAddrB: {
            ++vm->ip;
        } break;

        // ret
        case OpReturn: {} break;
        
        // leave
        case OpLeave: {} break;
        
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
            case OpHlt: {
                vm->halted = true;
            } break;

            // mov <constant>, <ptr>
            case OpMoveCA: {
                u16 value   = vm_read_u16(vm);
                u16 ptr     = vm_read_u16(vm);

                printf("ptr = 0x%04X\n", ptr);
                printf("value = 0x%02X\n", value);

                PEEK_RAM(vm, ptr) = value;

                printf("PEEK_RAM(vm, ptr) = 0x%02X\n", PEEK_RAM(vm, ptr));
            } break;
            // mov <constant>, <register>
            case OpMoveCR: {
                u16 value   = vm_read_u16(vm);
                u16* reg     = vm_get_register(vm, vm->data[vm->ip++]);

                *reg = value;
            } break;
            // mov <ptr>, <register>
            case OpMoveAR: {
                u16 ptr   = vm_read_u16(vm);
                u16* reg     = vm_get_register(vm, vm->data[vm->ip++]);

                *reg = PEEK_RAM(vm, ptr);
            } break;
            // mov <register>, <register>
            case OpMoveRR: {
                u16* regB     = vm_get_register(vm, vm->data[vm->ip++]);
                u16* regA     = vm_get_register(vm, vm->data[vm->ip++]);

                *regA = *regB;
            } break;

            case OpAddAC: {
                u16 ptr     = vm_read_u16(vm);
                vm->r[0] = PEEK_RAM(vm, ptr) + vm_read_u16(vm);
            } break;  // r0 = *addr  + const
            
            case OpAddAA: {
                u16 ptrA     = vm_read_u16(vm);
                u16 ptrB     = vm_read_u16(vm);

                vm->r[0] = PEEK_RAM(vm, ptrA) + PEEK_RAM(vm, ptrB); 
            } break;  // r0 = *addr1 + *addr2
            
            case OpAddRC: {
                u16* reg     = vm_get_register(vm, vm->data[vm->ip++]);
                vm->r[0] = *reg + vm_read_u16(vm);
            } break;  // r0 =  reg   + const

            case OpEqAA: {
                u16 ptrA     = vm_read_u16(vm);
                u16 ptrB     = vm_read_u16(vm);

                vm->r[0] = PEEK_RAM(vm, ptrA) == PEEK_RAM(vm, ptrB); 
            } break;   // r0 = *addr1 == *addr2
            
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
                vm->ip = vm->memory[vm->sp--];
            break;

            // leave
            case OpLeave:
                vm->sp = vm->bp;
                vm->bp = vm->memory[vm->sp--];
            break;

            // call <addr>
            case OpCall: {
                u16 addr = vm_read_u16(vm);
                vm->memory[vm->sp++] = vm->ip;
                vm->ip = addr;
            } break;
            
            // push <addr>
            case OpPushAddr: {
                u16 addr = vm_read_u16(vm);
                vm_pushU16_stack(vm, PEEK_RAM(vm, addr));
            } break;

            // push <register>
            case OpPushReg: {
                u16* reg = vm_get_register(vm, vm->data[vm->ip++]);
                vm_pushU16_stack(vm, *reg);
            } break;

            // pop <addr>
            case OpPopAddr: {
                u16 addr = vm_read_u16(vm);
                PEEK_RAM(vm, addr) = vm_popU16_stack(vm);
            } break;
            
            // push <register>
            case OpPopReg: {
                u16* reg = vm_get_register(vm, vm->data[vm->ip++]);
                *reg = vm_popU16_stack(vm);
            } break;
            
            // pushb <addr>
            case OpPushAddrB: {
                u16 addr = vm_read_u16(vm);
                vm_pushU8_stack(vm, PEEK_RAM(vm, addr));
            } break;

            // popb <addr>
            case OpPopAddrB: {
                u16 addr = vm_read_u16(vm);
                PEEK_RAM(vm, addr) = vm_popU8_stack(vm);
            } break;

            // sys
            case OpSyscall: {
                u16 sn = PEEK_RAM(vm, 0);
                printf("sn = %d\n", sn);
                switch(sn) {
                    // syscall 0x00 -> print character to stdout
                    case 0x00: {
                        char c = vm_popU8_stack(vm);
                        printf("c = %c\n", c);
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