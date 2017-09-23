.text
.globl tigermain
.type tigermain, @function

tigermain:
    pushl %ebp
    movl %esp, %ebp
    subl $12, %esp
    movl %ebx, -4(%ebp)
    movl %esi, -8(%ebp)
    movl %edi, -12(%ebp)
    movl $4, %edi
    movl $0, %ebx
L5:
    cmp %edi, %ebx
    jle L6
L1:
    movl $0, %eax
    movl -4(%ebp), %ebx
    movl -8(%ebp), %esi
    movl -12(%ebp), %edi
    jmp L7
L6:
    pushl %ebx
    call printi
    movl %eax, %esi
    addl $4, %esp
    movl $3, %eax
    cmp %eax, %ebx
    je L2
L3:
    addl $1, %ebx
    jmp L5
L2:
    jmp L1
L7:
    movl %ebp, %esp
    popl %ebp
    ret

.section .rodata
