.text
.globl tigermain
.type tigermain, @function

tigermain:
    pushl %ebp
    movl %esp, %ebp
    subl $4, %esp
    movl %edi, -4(%ebp)
    movl $10, %eax
    pushl %eax
    pushl %ebp
    call nfactor
    movl %eax, %edi
    addl $8, %esp
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

nfactor:
    pushl %ebp
    movl %esp, %ebp
    subl $12, %esp
    movl %ebx, -12(%ebp)
    movl %esi, -4(%ebp)
    movl %edi, -8(%ebp)
    movl 12(%ebp), %ebx
    movl $0, %eax
    cmp %eax, %ebx
    je L1
L2:
    movl 12(%ebp), %ebx
    movl 12(%ebp), %eax
    subl $1, %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call nfactor
    movl %eax, %edi
    addl $8, %esp
    imull %edi, %ebx
L3:
    movl %ebx, %eax
    movl -12(%ebp), %ebx
    movl -4(%ebp), %esi
    movl -8(%ebp), %edi
    jmp L5
L1:
    movl $1, %ebx
    jmp L3
L5:
    movl %ebp, %esp
    popl %ebp
    ret

.section .rodata
