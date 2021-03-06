.text
.globl tigermain
.type tigermain, @function

tigermain:
    pushl %ebp
    movl %esp, %ebp
    subl $28, %esp
    movl %ebx, -24(%ebp)
    movl %edi, -28(%ebp)
    movl $8, %eax
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
    movl %ebp, %edi
    addl $-12, %edi
    movl $0, %eax
    pushl %eax
    movl -4(%ebp), %eax
    pushl %eax
    call initArray
    movl %eax, %ebx
    addl $8, %esp
    movl %ebx, (%edi)
    movl %ebp, %edi
    addl $-16, %edi
    movl $0, %eax
    pushl %eax
    movl -4(%ebp), %eax
    movl -4(%ebp), %ebx
    addl %ebx, %eax
    subl $1, %eax
    pushl %eax
    call initArray
    movl %eax, %ebx
    addl $8, %esp
    movl %ebx, (%edi)
    movl %ebp, %edi
    addl $-20, %edi
    movl $0, %eax
    pushl %eax
    movl -4(%ebp), %eax
    movl -4(%ebp), %ebx
    addl %ebx, %eax
    subl $1, %eax
    pushl %eax
    call initArray
    movl %eax, %ebx
    addl $8, %esp
    movl %ebx, (%edi)
    movl $0, %eax
    pushl %eax
    pushl %ebp
    call try
    movl %eax, %ebx
    addl $8, %esp
    movl %ebx, %eax
    movl -24(%ebp), %ebx
    movl -28(%ebp), %edi
    jmp L33
L33:
    movl %ebp, %esp
    popl %ebp
    ret

try:
    pushl %ebp
    movl %esp, %ebp
    subl $12, %esp
    movl %ebx, -4(%ebp)
    movl %esi, -8(%ebp)
    movl %edi, -12(%ebp)
    movl 12(%ebp), %ebx
    movl 8(%ebp), %eax
    movl -4(%eax), %eax
    cmp %eax, %ebx
    je L30
L31:
    movl $0, %ebx
    movl 8(%ebp), %eax
    movl -4(%eax), %eax
    subl $1, %eax
    movl %eax, %esi
L28:
    cmp %esi, %ebx
    jle L29
L14:
    movl $0, %eax
L32:
    movl -4(%ebp), %ebx
    movl -8(%ebp), %esi
    movl -12(%ebp), %edi
    jmp L34
L30:
    movl 8(%ebp), %eax
    pushl %eax
    call printboard
    movl %eax, %ebx
    addl $4, %esp
    movl %ebx, %eax
    jmp L32
L29:
    movl 8(%ebp), %eax
    movl -8(%eax), %ecx
    movl %ebx, %eax
    imull $4, %eax
    addl %eax, %ecx
    movl (%ecx), %ecx
    movl $0, %eax
    cmp %eax, %ecx
    je L15
L16:
    movl $0, %ecx
L17:
    movl $0, %eax
    cmp %eax, %ecx
    jne L20
L21:
    movl $0, %ecx
L22:
    movl $0, %eax
    cmp %eax, %ecx
    jne L25
L26:
    movl %ebx, %eax
    addl $1, %eax
    movl %eax, %ebx
    jmp L28
L15:
    movl $1, %ecx
    movl 8(%ebp), %eax
    movl -16(%eax), %edi
    movl 12(%ebp), %edx
    movl %ebx, %eax
    addl %edx, %eax
    imull $4, %eax
    addl %eax, %edi
    movl (%edi), %edx
    movl $0, %eax
    cmp %eax, %edx
    je L18
L19:
    movl $0, %ecx
L18:
    jmp L17
L20:
    movl $1, %ecx
    movl 8(%ebp), %eax
    movl -20(%eax), %edi
    movl %ebx, %eax
    addl $7, %eax
    movl 12(%ebp), %edx
    subl %edx, %eax
    imull $4, %eax
    addl %eax, %edi
    movl (%edi), %edx
    movl $0, %eax
    cmp %eax, %edx
    je L23
L24:
    movl $0, %ecx
L23:
    jmp L22
L25:
    movl 8(%ebp), %eax
    movl -8(%eax), %ecx
    movl %ebx, %eax
    imull $4, %eax
    addl %eax, %ecx
    movl $1, %eax
    movl %eax, (%ecx)
    movl 8(%ebp), %eax
    movl -16(%eax), %ecx
    movl 12(%ebp), %edx
    movl %ebx, %eax
    addl %edx, %eax
    imull $4, %eax
    addl %eax, %ecx
    movl $1, %eax
    movl %eax, (%ecx)
    movl 8(%ebp), %eax
    movl -20(%eax), %ecx
    movl %ebx, %eax
    addl $7, %eax
    movl 12(%ebp), %edx
    subl %edx, %eax
    imull $4, %eax
    addl %eax, %ecx
    movl $1, %eax
    movl %eax, (%ecx)
    movl 8(%ebp), %eax
    movl -12(%eax), %ecx
    movl 12(%ebp), %eax
    imull $4, %eax
    addl %eax, %ecx
    movl %ebx, (%ecx)
    movl 12(%ebp), %eax
    addl $1, %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call try
    movl %eax, %edi
    addl $8, %esp
    movl 8(%ebp), %eax
    movl -8(%eax), %ecx
    movl %ebx, %eax
    imull $4, %eax
    addl %eax, %ecx
    movl $0, %eax
    movl %eax, (%ecx)
    movl 8(%ebp), %eax
    movl -16(%eax), %ecx
    movl 12(%ebp), %edx
    movl %ebx, %eax
    addl %edx, %eax
    imull $4, %eax
    addl %eax, %ecx
    movl $0, %eax
    movl %eax, (%ecx)
    movl 8(%ebp), %eax
    movl -20(%eax), %ecx
    movl %ebx, %eax
    addl $7, %eax
    movl 12(%ebp), %edx
    subl %edx, %eax
    imull $4, %eax
    addl %eax, %ecx
    movl $0, %eax
    movl %eax, (%ecx)
    jmp L26
L34:
    movl %ebp, %esp
    popl %ebp
    ret

printboard:
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
    jmp L35
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
    movl 8(%ebp), %eax
    movl -12(%eax), %ecx
    movl %esi, %eax
    imull $4, %eax
    addl %eax, %ecx
    movl (%ecx), %ecx
    movl -28(%ebp), %eax
    cmp %eax, %ecx
    je L5
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
L35:
    movl %ebp, %esp
    popl %ebp
    ret

.section .rodata
L13:
.string "   \n"
L10:
.string "   \n"
L4:
.string "    ."
L3:
.string "    O"
