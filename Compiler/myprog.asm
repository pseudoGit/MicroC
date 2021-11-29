  global main
  extern printf

  segment .bss
  sum resq 1
  a2 resq 1
  a1 resq 1
  a resq 1

  section .text
main:
  mov rax,1
  push rax
  pop qword[i]
  mov rax,0
  push rax
  pop qword[sum]
L2:
  push qword[i]
  mov rax,10
  push rax
  pop rbx
  pop rax
  cmp rax,rbx
  jl J1
  mov rax,0
  jmp J2
J1:
  mov rax,1
J2:
  push rax
  pop rax
  cmp rax,0
  je L3
  push qword[sum]
  push qword[i]
  call add
  push rax
  pop qword[sum]
  push qword[i]
  push qword[sum]
  mov    rdi,fmt1
  pop   rdx
  pop   rsi
  mov    rax,0
  push   rbp
  call   printf
  pop    rbp
  push qword[i]
  mov rax,1
  push rax
  call add
  push rax
  pop qword[i]
  jmp L2
L3:
  mov rax,0
  push rax
  pop rax
  ret
add:
  pop r15
  pop qword[a2]
  pop qword[a1]
  push qword[a1]
  push qword[a2]
  pop rbx
  pop rax
  add rax,rbx
  push rax
  pop qword[a]
  push qword[a]
  pop rax
  push r15
  ret

  section .data
  fmt1: db `"i = %d sum = %d\n"`, 0
