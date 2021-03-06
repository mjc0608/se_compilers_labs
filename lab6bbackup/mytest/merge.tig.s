.text
.globl tigermain
.type tigermain, @function

tigermain:
    pushl %ebp
    movl %esp, %ebp
    subl $12, %esp
    movl %ebx, %eax
    movl %eax, -8(%ebp)    #swap t107 out with t121
    movl %esi, %esi
    movl %edi, %eax
    movl %eax, -12(%ebp)    #swap t109 out with t122
    movl %ebp, %eax
    addl $-4, %eax
    movl %eax, %edi
    call getchar
    movl %eax, %ebx
    movl %ebx, %eax
    movl %eax, (%edi)
    pushl %ebp
    call skipto
    movl %eax, %ebx
    movl -4(%ebp), %eax
    pushl %eax
    call print
    movl %eax, %ebx
    movl %ebx, %eax
    movl -8(%ebp), %ebx    #swap t107 in with t123
    movl %ebx, %ebx
    movl %esi, %esi
    movl -12(%ebp), %ebx    #swap t109 in with t124
    movl %ebx, %edi
    movl %eax, %eax
    jmp L5
L5:
    movl %ebp, %esp
    popl %ebp
    ret

skipto:
    pushl %ebp
    movl %esp, %ebp
    subl $8, %esp
    movl %ebx, %eax
    movl %eax, -4(%ebp)    #swap t101 out with t136
    movl %esi, %esi
    movl %edi, %eax
    movl %eax, -8(%ebp)    #swap t103 out with t137
L3:
    movl $L1, %eax
    pushl %eax
    movl 8(%ebp), %eax
    movl -4(%eax), %eax
    pushl %eax
    call stringEqual
    movl %eax, %ebx
    movl %ebx, %ebx
    movl $0, %eax
    cmp %eax, %ebx
    jne L4
L2:
    movl -4(%ebp), %eax    #swap t101 in with t138
    movl %eax, %ebx
    movl %esi, %esi
    movl -8(%ebp), %eax    #swap t103 in with t139
    movl %eax, %edi
    jmp L6
L4:
    movl 8(%ebp), %eax
    movl %eax, %eax
    addl $-4, %eax
    movl %eax, %edi
    call getchar
    movl %eax, %ebx
    movl %ebx, %eax
    movl %eax, (%edi)
    jmp L3
L6:
    movl %ebp, %esp
    popl %ebp
    ret

.section .rodata
L1:
.string "    "
