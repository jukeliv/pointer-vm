#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "vm.h"

#define ARRSIZE(arr) (sizeof(arr)/sizeof(*arr))

typedef struct token token;

typedef struct patch patch;

enum token_type {
    TokenEq,   // eq <Cptr>, <Aptr>, <Bptr>
    TokenAdd,  // add <Cptr>, <Aptr>, <Bptr>
    TokenMov,  // mov <value>, <addr>
    TokenNull, // null <addr> => mov 0, <addr>
    TokenPush, // push <addr>
    TokenPushB,// pushb <addr>
    TokenPop,  // pop <addr>
    TokenPopB, // popb <addr>
    TokenJmp,  // jmp <addr>
    TokenIf,   // if <addr>
    TokenCall, // call <addr>
    TokenHalt, // halt
    TokenSys,  // sys
    TokenRet,  // ret
    TokenNumber,
    TokenComma,
    TokenSymbol, // $<id>
    TokenEOF,
};

struct token {
    u8 type;
    union {
        u16 data;
        char* symbol;
    };
};

enum patch_type {
    PATCH_DECL, // when we find `$<id>` alone, we know is a declaration
    PATCH_REF,  // when we find `$<id>` in a instruction like `jmp $<id>`, we know is a reference
};

struct patch {
    u8 type;
    u16 addr;
    char* id;
};

patch patches[0xFF] = {0};
size_t patches_sp = 0;

void patches_push(u8 type, u16 addr, char* id) {
    patches[patches_sp].type = type;
    patches[patches_sp].addr = addr;
    patches[patches_sp].id = id;
    ++patches_sp;
}

vm_t gen_bytecode(token* tokens) {
    vm_t vm = {0};

    u16 i = 0;

    while(tokens[i].type != TokenEOF) {
        token token = tokens[i++];
        switch(token.type) {
            case TokenSymbol: {
                printf("vm.ip = 0x%04X\n", vm.ip);
                patches_push(PATCH_DECL, vm.ip, token.symbol);
            } break;
            case TokenHalt: {
                vm.data[vm.ip++] = OpHalt;
            } break;
            case TokenSys: {
                vm.data[vm.ip++] = OpSyscall;
            } break;
            case TokenEq:{
                // eq <Cptr>, <Aptr>, <Bptr>
                u16 c = tokens[i++].data;
                tokens[i++]; // Skip the comma
                u16 a = tokens[i++].data;
                tokens[i++]; // Skip the comma
                u16 b = tokens[i++].data;

                vm.data[vm.ip++] = OpEq;
                // Cptr
                *(u16*)(vm.data+vm.ip) = c;
                vm.ip += 2;
                // Aptr
                *(u16*)(vm.data+vm.ip) = a;
                vm.ip += 2;
                // Bptr
                *(u16*)(vm.data+vm.ip) = b;
                vm.ip += 2;
            } break;
            case TokenAdd:{
                // add <Cptr>, <Aptr>, <Bptr>
                u16 c = tokens[i++].data;
                tokens[i++]; // Skip the comma
                u16 a = tokens[i++].data;
                tokens[i++]; // Skip the comma
                u16 b = tokens[i++].data;

                vm.data[vm.ip++] = OpAdd;
                // Cptr
                *(u16*)(vm.data+vm.ip) = c;
                vm.ip += 2;
                // Aptr
                *(u16*)(vm.data+vm.ip) = a;
                vm.ip += 2;
                // Bptr
                *(u16*)(vm.data+vm.ip) = b;
                vm.ip += 2;
            } break;
            case TokenMov: {
                // mov <value>, <to>
                
                vm.data[vm.ip++] = OpMove;
                
                // value
                switch(tokens[i].type) {
                    case TokenNumber: {
                        u16 value = tokens[i++].data;
                        *(u16*)(vm.data+vm.ip) = value;
                    }break;
                    case TokenSymbol:
                        patches_push(PATCH_REF, vm.ip, tokens[i++].symbol);
                    break;
                    default: todo("Figure out what to error here!"); break;
                }
                vm.ip += 2;

                tokens[i++]; // Skip the comma

                // to
                u16 to = tokens[i++].data;
                *(u16*)(vm.data+vm.ip) = to;
                vm.ip += 2;
            } break;

            case TokenNull: {
                u16 addr = tokens[i++].data;

                vm.data[vm.ip++] = OpMove;
                // value
                *(u16*)(vm.data+vm.ip) = 0;
                vm.ip += 2;
                // to
                *(u16*)(vm.data+vm.ip) = addr;
                vm.ip += 2;
            } break;
            case TokenPush: {
                u16 addr = tokens[i++].data;

                vm.data[vm.ip++] = OpPush;
                
                *(u16*)(vm.data+vm.ip) = addr;
                vm.ip += 2;
            } break;
            case TokenPushB: {
                u16 addr = tokens[i++].data;

                vm.data[vm.ip++] = OpPushB;
                
                *(u16*)(vm.data+vm.ip) = addr;
                vm.ip += 2;
            } break;
            case TokenPop: {
                u16 addr = tokens[i++].data;

                vm.data[vm.ip++] = OpPop;
                
                *(u16*)(vm.data+vm.ip) = addr;
                vm.ip += 2;
            }break;
            case TokenPopB:{
                u16 addr = tokens[i++].data;

                vm.data[vm.ip++] = OpPopB;
                
                *(u16*)(vm.data+vm.ip) = addr;
                vm.ip += 2;
            }break;
            case TokenJmp:{
                vm.data[vm.ip++] = OpJmp;

                switch(tokens[i].type) {
                    case TokenNumber:
                        *(u16*)(vm.data+vm.ip) = tokens[i].data;
                    break;
                    case TokenSymbol:
                        patches_push(PATCH_REF, vm.ip, tokens[i].symbol);
                    break;
                    default: todo("Figure out a better error message!"); break;
                }
                vm.ip += 2;
                ++i;
            }break;
            case TokenIf:{
                u16 addr = tokens[i++].data;

                vm.data[vm.ip++] = OpIf;

                *(u16*)(vm.data+vm.ip) = addr;
                vm.ip += 2;
            }break;

            case TokenCall:
                vm.data[vm.ip++] = OpCall;

                switch(tokens[i].type) {
                    case TokenNumber:
                        *(u16*)(vm.data+vm.ip) = tokens[i++].data;
                    break;
                    case TokenSymbol:
                        patches_push(PATCH_REF, vm.ip, tokens[i++].symbol);
                    break;
                    default: todo("Figure out a better error message!"); break;
                }
                vm.ip += 2;
            break;

            case TokenRet:
                vm.data[vm.ip++] = OpReturn;
            break;

            default:
                printf("token_type = 0x%02X\n", tokens[i-1].type);
                printf("i = %zu\n", i-1);
                todo("Implement not implemented token!");
            break;
        }
    }

    for(u8 i = 0; i < patches_sp; ++i) {
        if(patches[i].type != PATCH_REF)
            continue;
        for(u8 j = 0; j < patches_sp; ++j) {
            if(patches[j].type != PATCH_DECL)
                continue;
            if(strcmp(patches[i].id, patches[j].id))
                continue;
            
            printf("patches[i].addr = 0x%04X\n", patches[i].addr);
            printf("patches[j].addr = 0x%04X\n", patches[j].addr);
            *(u16*)(vm.data+patches[i].addr) = patches[j].addr;
            printf("*(u16*)(vm.data+0x%04X) = 0x%04X\n", patches[i].addr, *(u16*)(vm.data+patches[i].addr));
        }
    }

    vm.ip = 0;
    return vm;
}

char* read_file(const char* path) {
    FILE* fp = fopen(path, "rb");

    fseek(fp, 0, SEEK_END);
    size_t fs = ftell(fp);
    rewind(fp);

    char* buf = malloc(fs+1);
    if(fread(buf, sizeof(char), fs, fp) != fs)
    {
        printf("ERROR: Coudln't read file %s\n", path);
        return NULL;
    }

    buf[fs] = 0;
    fclose(fp);
    
    return buf;
}

void print_tokens(token* tokens, u16 tokens_size) {
    for(u16 i = 0; i < tokens_size; ++i) {
        printf("tokens[%d] = {\n\t.type = 0x%02X,\n\t.data = %u\n}\n", i, tokens[i].type, tokens[i].data);
    }
}

int main(int argc, char** argv) {
    token tokens[0xFFF] = {0};
    u16 tokens_size = 0;

    if (argc < 3) {
        return 1;
    }

    char* input_file = argv[1];
    char* output_file = argv[2];
    
    char* file_content = read_file(input_file);
    size_t file_size = strlen(file_content);

    char lexeme[256] = {0};
    unsigned char li = 0;

    {
        size_t i = 0;
        while(i < file_size) {
            char c = file_content[i];
            if(iswspace(c)) {
                ++i;
                continue;
            }

            printf("c = %c\n", c);
            switch(c) {
                case ';':
                    while(i < file_size &&
                        (c = file_content[i++]) != '\n');
                break;
                case '\'': {
                    ++i;
                    char ASCII = file_content[i++];
                    if((c = file_content[i++]) != '\'') {
                        todo("Figure out what to do when a user inputs more than one character into an ASCII ' '");
                    }
                    tokens[tokens_size++] = (token) {
                        .type = TokenNumber,
                        .data = ASCII
                    };
                    // ++i;
                } break;
                case ',':
                    tokens[tokens_size++] = (token) {
                        .type = TokenComma,
                    };
                    ++i;
                break;
                case '0':
                    if(file_content[i+1] == 'x') {
                        li = 0;
                        memset(lexeme, 0, ARRSIZE(lexeme));
                        i += 2;
                        lexeme[li++] = '0';
                        lexeme[li++] = 'x';
                        while(i < file_size &&
                            isxdigit((c = file_content[i++]))) {
                            lexeme[li++] = c;
                        }
                        lexeme[li] = 0;

                        tokens[tokens_size++] = (token) {
                            .type = TokenNumber,
                            .data = (u16)strtol(lexeme, NULL, 16)
                        };
                        break;
                    }
                case '#':{
                    ++i;
                    li = 0;
                    memset(lexeme, 0, ARRSIZE(lexeme));
                    while(i < file_size &&
                        isalpha((c = file_content[i++])) || c == '_') {
                        lexeme[li++] = c;
                    }
                    lexeme[li] = 0;

                    tokens[tokens_size++] = (token) {
                        .type = TokenSymbol,
                        .symbol = strdup(lexeme)
                    };
                }break;
                default:
                    li = 0;
                    memset(lexeme, 0, ARRSIZE(lexeme));
                    if(isalpha(c)) {
                        while(i < file_size &&
                            isalpha((c = file_content[i++]))) {
                            lexeme[li++] = c;
                        }
                        lexeme[li] = 0;

                        u8 type = 0;

                        if(!strcmp(lexeme, "mov"))
                            type = TokenMov;
                        else if(!strcmp(lexeme, "eq"))
                            type = TokenEq;
                        else if(!strcmp(lexeme, "add"))
                            type = TokenAdd;
                        else if(!strcmp(lexeme, "null"))
                            type = TokenNull;
                        else if(!strcmp(lexeme, "hlt"))
                            type = TokenHalt;
                        else if(!strcmp(lexeme, "push"))
                            type = TokenPush;
                        else if(!strcmp(lexeme, "pushb"))
                            type = TokenPushB;
                        else if(!strcmp(lexeme, "pop"))
                            type = TokenPop;
                        else if(!strcmp(lexeme, "popb"))
                            type = TokenPopB;
                        else if(!strcmp(lexeme, "jmp"))
                            type = TokenJmp;
                        else if(!strcmp(lexeme, "if"))
                            type = TokenIf;
                        else if(!strcmp(lexeme, "ret"))
                            type = TokenRet;
                        else if(!strcmp(lexeme, "call"))
                            type = TokenCall;
                        else if(!strcmp(lexeme, "sys"))
                            type = TokenSys;
                        else {
                            printf("Unknown lexeme found! ( %s )\n", lexeme);
                            exit(1);
                        }
                        
                        tokens[tokens_size++] = (token) {
                            .type = type
                        };
                    } else if(isdigit(c)) {
                        while(i < file_size &&
                            isdigit((c = file_content[i++]))) {
                            lexeme[li++] = c;
                        }
                        lexeme[li] = 0;

                        tokens[tokens_size++] = (token) {
                            .type = TokenNumber,
                            .data = (u16)atoi(lexeme)
                        };
                    } else {
                        todo("Figure out what to do when we find an unknown token");
                    }
                break;
            }
        }
        tokens[tokens_size++] = (token) {
            .type = TokenEOF
        };
    }
    
    printf("tokens generated!\n");
    
    print_tokens(tokens, tokens_size);

    vm_t vm = gen_bytecode(tokens);

    for(u8 i = 0; i < 10; ++i)
        printf("| 0x%02X | ", vm.data[i]);
    putchar('\n');

    printf("bytecode generated!\n");

    FILE* fp = fopen(output_file, "wb");
    // store only the ROM onto the file
    fwrite(vm.data, sizeof(*vm.data), sizeof(vm.data)/sizeof(*vm.data), fp);

    fclose(fp);

    // vm_dump_memory(&vm, 4);

    return 0;
}