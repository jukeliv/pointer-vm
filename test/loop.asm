call #main hlt
#main

    ; u16 i = 0 
    null 0x04   
    ; while(true)
    #loop

    mov 'E', 0x00
    pushb 0x00
    null 0x00
    sys

    ; i = i + 1
    mov 1 , 0x06
    add 0x04 , 0x04 , 0x06

    ; if(i == 10)
    mov 10 , 0x06
    eq 0x06 , 0x04 , 0x06
    if 0x06 ; break
    jmp #loop

    ret