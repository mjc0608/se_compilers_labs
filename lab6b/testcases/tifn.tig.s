.text
.globl tigermain
.type tigermain, @function

tigermain:
    pushl %ebp
    movl %esp, %ebp
    subl $8, %esp
    movl %edi, -8(%ebp)
    movl $5, %eax
    movl %eax, -4(%ebp)
    movl -4(%ebp), %eax
    pushl %eax
    call printi
    movl %eax, %edi
    addl $4, %esp
    movl $L7, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    movl $2, %eax
    pushl %eax
    pushl %ebp
    call g
    movl %eax, %edi
    addl $8, %esp
    movl -4(%ebp), %eax
    pushl %eax
    call printi
    movl %eax, %edi
    addl $4, %esp
    movl %edi, %eax
    movl -8(%ebp), %edi
    jmp L8
L8:
    movl %ebp, %esp
    popl %ebp
    ret

g:
    pushl %ebp
    movl %esp, %ebp
    subl $4, %esp
    movl %edi, -4(%ebp)
    movl $1, %ecx
    movl 12(%ebp), %edx
    movl $3, %eax
    cmp %eax, %edx
    jg L1
L2:
    movl $0, %ecx
L1:
    movl $0, %eax
    cmp %eax, %ecx
    jne L4
L5:
    movl 8(%ebp), %ecx
    movl $4, %eax
    movl %eax, -4(%ecx)
    movl $0, %eax
L6:
    movl -4(%ebp), %edi
    jmp L9
L4:
    movl $L3, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    movl %edi, %eax
    jmp L6
L9:
    movl %ebp, %esp
    popl %ebp
    ret

.section .rodata
L7:
.string "   \n"
L3:
.string "   hey! Bigger than 3!\n"
