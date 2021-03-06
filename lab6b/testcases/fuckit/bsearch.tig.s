.text
.globl tigermain
.type tigermain, @function

tigermain:
    pushl %ebp
    movl %esp, %ebp
    subl $16, %esp
    movl %ebx, -12(%ebp)
    movl %edi, -16(%ebp)
    movl $16, %eax
    movl %eax, -4(%ebp)
    movl %ebp, %edi
    addl $-8, %edi
    movl $0, %eax
    pushl %eax
    movl -4(%ebp), %eax
    pushl %eax
    call initArray
    movl %eax, %ebx
    addl $8, %esp
    movl %ebx, (%edi)
    pushl %ebp
    call try
    movl %eax, %ebx
    addl $4, %esp
    movl %ebx, %eax
    movl -12(%ebp), %ebx
    movl -16(%ebp), %edi
    jmp L12
L12:
    movl %ebp, %esp
    popl %ebp
    ret

try:
    pushl %ebp
    movl %esp, %ebp
    subl $4, %esp
    movl %edi, -4(%ebp)
    movl 8(%ebp), %eax
    pushl %eax
    call init
    movl %eax, %edi
    addl $4, %esp
    movl $7, %eax
    pushl %eax
    movl 8(%ebp), %eax
    movl -4(%eax), %eax
    subl $1, %eax
    pushl %eax
    movl $0, %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call bsearch
    movl %eax, %edi
    addl $16, %esp
    pushl %edi
    call printi
    movl %eax, %edi
    addl $4, %esp
    movl $L11, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    movl %edi, %eax
    movl -4(%ebp), %edi
    jmp L13
L13:
    movl %ebp, %esp
    popl %ebp
    ret

bsearch:
    pushl %ebp
    movl %esp, %ebp
    subl $4, %esp
    movl %edi, -4(%ebp)
    movl 12(%ebp), %ecx
    movl 16(%ebp), %eax
    cmp %eax, %ecx
    je L8
L9:
    movl 12(%ebp), %ecx
    movl 16(%ebp), %eax
    addl %eax, %ecx
    movl $2, %edi
    movl %ecx, %eax
    cltd
    idivl %edi
    movl 8(%ebp), %ecx
    movl -8(%ecx), %edx
    movl %eax, %ecx
    imull $4, %ecx
    addl %ecx, %edx
    movl (%edx), %edx
    movl 20(%ebp), %ecx
    cmp %ecx, %edx
    jl L5
L6:
    movl 20(%ebp), %ecx
    pushl %ecx
    pushl %eax
    movl 12(%ebp), %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call bsearch
    movl %eax, %edi
    addl $16, %esp
L7:
L10:
    movl %edi, %eax
    movl -4(%ebp), %edi
    jmp L14
L8:
    movl 12(%ebp), %edi
    jmp L10
L5:
    movl 20(%ebp), %ecx
    pushl %ecx
    movl 16(%ebp), %ecx
    pushl %ecx
    addl $1, %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call bsearch
    movl %eax, %edi
    addl $16, %esp
    jmp L7
L14:
    movl %ebp, %esp
    popl %ebp
    ret

init:
    pushl %ebp
    movl %esp, %ebp
    subl $12, %esp
    movl %ebx, -4(%ebp)
    movl %esi, -8(%ebp)
    movl %edi, -12(%ebp)
    movl $0, %ebx
    movl 8(%ebp), %eax
    movl -4(%eax), %edi
    subl $1, %edi
L3:
    cmp %edi, %ebx
    jle L4
L2:
    movl $0, %eax
    movl -4(%ebp), %ebx
    movl -8(%ebp), %esi
    movl -12(%ebp), %edi
    jmp L15
L4:
    movl 8(%ebp), %eax
    movl -8(%eax), %ecx
    movl %ebx, %eax
    imull $4, %eax
    addl %eax, %ecx
    movl %ebx, %eax
    imull $2, %eax
    addl $1, %eax
    movl %eax, (%ecx)
    movl 8(%ebp), %eax
    pushl %eax
    call nop
    movl %eax, %esi
    addl $4, %esp
    addl $1, %ebx
    jmp L3
L15:
    movl %ebp, %esp
    popl %ebp
    ret

nop:
    pushl %ebp
    movl %esp, %ebp
    subl $4, %esp
    movl %edi, -4(%ebp)
    movl $L1, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    movl %edi, %eax
    movl -4(%ebp), %edi
    jmp L16
L16:
    movl %ebp, %esp
    popl %ebp
    ret

.section .rodata
L11:
.string "   \n"
L1:
.string "    "
