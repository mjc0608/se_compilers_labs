.text
.globl tigermain
.type tigermain, @function

tigermain:
    pushl %ebp
    movl %esp, %ebp
    subl $4, %esp
    movl %edi, -4(%ebp)
    movl $2, %eax
    pushl %eax
    pushl %ebp
    call a
    movl %eax, %edi
    addl $8, %esp
    pushl %edi
    call printi
    movl %eax, %edi
    addl $4, %esp
    movl %edi, %eax
    movl -4(%ebp), %edi
    jmp L1
L1:
    movl %ebp, %esp
    popl %ebp
    ret

a:
    pushl %ebp
    movl %esp, %ebp
    subl $4, %esp
    movl %edi, -4(%ebp)
    movl $3, %eax
    pushl %eax
    pushl %ebp
    call b
    movl %eax, %edi
    addl $8, %esp
    movl %edi, %eax
    movl -4(%ebp), %edi
    jmp L2
L2:
    movl %ebp, %esp
    popl %ebp
    ret

b:
    pushl %ebp
    movl %esp, %ebp
    movl 8(%ebp), %eax
    movl 12(%eax), %eax
    movl 12(%ebp), %ecx
    addl %ecx, %eax
    jmp L3
L3:
    movl %ebp, %esp
    popl %ebp
    ret

.section .rodata
