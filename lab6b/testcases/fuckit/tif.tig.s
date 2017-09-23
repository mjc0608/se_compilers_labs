.text
.globl tigermain
.type tigermain, @function

tigermain:
    pushl %ebp
    movl %esp, %ebp
    subl $4, %esp
    movl %edi, -4(%ebp)
    movl $4, %eax
    pushl %eax
    movl $9, %eax
    pushl %eax
    pushl %ebp
    call g
    movl %eax, %edi
    addl $12, %esp
    pushl %edi
    call printi
    movl %eax, %edi
    addl $4, %esp
    movl %edi, %eax
    movl -4(%ebp), %edi
    jmp L4
L4:
    movl %ebp, %esp
    popl %ebp
    ret

g:
    pushl %ebp
    movl %esp, %ebp
    movl 12(%ebp), %ecx
    movl 16(%ebp), %eax
    cmp %eax, %ecx
    jg L1
L2:
    movl 16(%ebp), %eax
L3:
    jmp L5
L1:
    movl 12(%ebp), %eax
    jmp L3
L5:
    movl %ebp, %esp
    popl %ebp
    ret

.section .rodata
