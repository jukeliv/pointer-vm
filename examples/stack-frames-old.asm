jmp %entry

%foo
  ; current stack layout
  ; none <- rsp
  ; *rip <- rbp
  push rbp

  ; current stack layout
  ; none <- rsp
  ; *rbp
  ; *rip <- rbp
  mov rsp , rbp
  
  ; current stack layout
  ; none <- rsp & rbp
  ; *rbp
  ; *rip

 ; reserve 2 bytes
  add rsp , 2
  mov r0 , rsp

  ; set rsp to be at rbp and pop rbp
  leave
  ; pop rip and return to where we saved
  ret
%entry
  call %foo ; push rip to the stack
  hlt