.text
.globl tigermain
.type tigermain, @function

tigermain:
    pushl %ebp
    movl %esp, %ebp
    subl $12, %esp
    movl %ebx, -12(%ebp)
    movl %esi, -4(%ebp)
    movl %edi, -8(%ebp)
    movl $10, %ebx
L5:
    movl $0, %eax
    cmp %eax, %ebx
    jge L6
L1:
    movl $0, %eax
    movl -12(%ebp), %ebx
    movl -4(%ebp), %esi
    movl -8(%ebp), %edi
    jmp L7
L6:
    pushl %ebx
    call printi
    movl %eax, %edi
    addl $4, %esp
    subl $1, %ebx
    movl $2, %eax
    cmp %eax, %ebx
    je L2
L3:
    jmp L5
L2:
    jmp L1
L7:
    movl %ebp, %esp
    popl %ebp
    ret

.section .rodata
