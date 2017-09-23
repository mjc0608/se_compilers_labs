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
    movl $8, %eax
    pushl %eax
    call allocRecord
    movl %eax, %ebx
    addl $4, %esp
    movl $3, %eax
    movl %eax, 0(%ebx)
    movl $4, %eax
    movl %eax, 4(%ebx)
    movl 0(%ebx), %eax
    pushl %eax
    call printi
    movl %eax, %edi
    addl $4, %esp
    movl 4(%ebx), %eax
    pushl %eax
    call printi
    movl %eax, %ebx
    addl $4, %esp
    movl %ebx, %eax
    movl -12(%ebp), %ebx
    movl -4(%ebp), %esi
    movl -8(%ebp), %edi
    jmp L1
L1:
    movl %ebp, %esp
    popl %ebp
    ret

.section .rodata
