  global main
  extern printf

  segment .bss
  i resq 1
  n resq 1
  factorial resq 1

  section .text
main:
  mov rax,1
  push rax
  pop qword[i]
  mov    rdi,fmt1
  mov    rax,0
  push   rbp
  call   printf
  pop    rbp
L2:
  push qword[i]
  mov rax,10
  push rax
  pop rbx
  pop rax
  cmp rax,rbx
  jle J1
  mov rax,0
  jmp J2
J1:
  mov rax,1
J2:
  push rax
  pop rax
  cmp rax,0
  je L3
  push qword[i]
  push qword[i]
  call fact
  push rax
  mov    rdi,fmt2
  pop   rdx
  pop   rsi
  mov    rax,0
  push   rbp
  call   printf
  pop    rbp
  jmp L2
L3:
  mov rax,0
  push rax
  pop rax
  ret
fact:
  pop r15
  pop qword[n]
  mov rax,1
  push rax
  pop qword[i]
  mov rax,1
  push rax
  pop qword[factorial]
L4:
  push qword[i]
  push qword[n]
  pop rbx
  pop rax
  cmp rax,rbx
  jle J3
  mov rax,0
  jmp J4
J3:
  mov rax,1
J4:
  push rax
  pop rax
  cmp rax,0
  je L5
  push qword[factorial]
  push qword[i]
  pop rbx
  pop rax
  imul rbx
  push rax
  pop qword[factorial]
  push qword[i]
  mov rax,1
  push rax
  pop rbx
  pop rax
  add rax,rbx
  push rax
  pop qword[i]
  jmp L4
L5:
  push qword[factorial]
  pop rax
  push r15
  ret

  section .data
  fmt1: db `" n    n!\n"`, 0
  fmt2: db `"%2d    %d\n"`, 0
