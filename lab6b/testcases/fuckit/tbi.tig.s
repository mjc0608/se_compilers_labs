.text
.globl tigermain
.type tigermain, @function

tigermain:
    pushl %ebp
    movl %esp, %ebp
    subl $8, %esp
    movl %edi, -8(%ebp)
    movl $8, %eax
    movl %eax, -4(%ebp)
    pushl %ebp
    call printb
    movl %eax, %edi
    addl $4, %esp
    movl %edi, %eax
    movl -8(%ebp), %edi
    jmp L14
L14:
    movl %ebp, %esp
    popl %ebp
    ret

printb:
    pushl %ebp
    movl %esp, %ebp
    subl $28, %esp
    movl %ebx, -4(%ebp)
    movl %esi, -8(%ebp)
    movl %edi, -12(%ebp)
    movl $0, %esi
    movl 8(%ebp), %eax
    movl -4(%eax), %eax
    subl $1, %eax
    movl %eax, -16(%ebp)
L11:
    movl -16(%ebp), %eax
    cmp %eax, %esi
    jle L12
L1:
    movl $L13, %eax
    pushl %eax
    call print
    movl %eax, %ebx
    addl $4, %esp
    movl %ebx, %eax
    movl -4(%ebp), %ebx
    movl -8(%ebp), %esi
    movl -12(%ebp), %edi
    jmp L15
L12:
    movl $0, %eax
    movl %eax, -28(%ebp)
    movl 8(%ebp), %eax
    movl -4(%eax), %ebx
    subl $1, %ebx
L8:
    movl -28(%ebp), %eax
    cmp %ebx, %eax
    jle L9
L2:
    movl $L10, %eax
    pushl %eax
    call print
    movl %eax, %ebx
    addl $4, %esp
    addl $1, %esi
    jmp L11
L9:
    movl -28(%ebp), %eax
    cmp %eax, %esi
    jg L5
L6:
    movl $L4, %eax
L7:
    pushl %eax
    call print
    movl %eax, %edi
    movl %edi, -24(%ebp)
    movl -24(%ebp), %edi
    movl %edi, -20(%ebp)
    addl $4, %esp
    movl -28(%ebp), %eax
    addl $1, %eax
    movl %eax, -28(%ebp)
    jmp L8
L5:
    movl $L3, %eax
    jmp L7
L15:
    movl %ebp, %esp
    popl %ebp
    ret

.section .rodata
L13:
.string "   \n"
L10:
.string "   \n"
L4:
.string "   y"
L3:
.string "   x"
