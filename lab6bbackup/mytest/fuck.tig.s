.text
.globl tigermain
.type tigermain, @function

tigermain:
    pushl %ebp
    movl %esp, %ebp
    subl $12, %esp
    movl %ebx, %eax
    movl %eax, -8(%ebp)    #swap t101 out with t122
    movl %esi, %esi
    movl %edi, %eax
    movl %eax, -12(%ebp)    #swap t103 out with t123
    movl %ebp, %eax
    addl $-4, %eax
    movl %eax, %edi
    call __wrap_getchar
    movl %eax, %ebx
    movl %ebx, %eax
    movl %eax, (%edi)
    movl $L1, %eax
    pushl %eax
    movl -4(%ebp), %eax
    pushl %eax
    call stringEqual
    movl %eax, %ebx
    movl %ebx, %ebx
    movl $0, %eax
    cmp %eax, %ebx
    jne L2
L3:
    movl $0, %eax
    movl -8(%ebp), %ebx    #swap t101 in with t124
    movl %ebx, %ebx
    movl %esi, %esi
    movl -12(%ebp), %ebx    #swap t103 in with t125
    movl %ebx, %edi
    movl %eax, %eax
    jmp L5
L2:
    movl $111, %eax
    pushl %eax
    call printi
    movl %eax, %ebx
    jmp L3
L5:
    movl %ebp, %esp
    popl %ebp
    ret

.section .rodata
L1:
.string "   0"
