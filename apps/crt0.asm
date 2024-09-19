; This file is part of the Sphynx OS
; It is released under the MIT license -- see LICENSE
; Written by: Kevin Alavik.

section .text
global _start

extern main

_start:
    call main
    
    ; exit(rax);
    mov rdi, rax
    mov rax, 2
    int 0x80

    
