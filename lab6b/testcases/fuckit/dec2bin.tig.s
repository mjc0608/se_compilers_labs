.text
.globl tigermain
.type tigermain, @function

tigermain:
    pushl %ebp
    movl %esp, %ebp
    subl $4, %esp
    movl %edi, -4(%ebp)
    pushl %ebp
    call try
    movl %eax, %edi
    addl $4, %esp
    movl %edi, %eax
    movl -4(%ebp), %edi
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
    movl $100, %eax
    pushl %eax
    call printi
    movl %eax, %edi
    addl $4, %esp
    movl $L4, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    movl $100, %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call dec2bin
    movl %eax, %edi
    addl $8, %esp
    movl $L5, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    movl $200, %eax
    pushl %eax
    call printi
    movl %eax, %edi
    addl $4, %esp
    movl $L6, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    movl $200, %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call dec2bin
    movl %eax, %edi
    addl $8, %esp
    movl $L7, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    movl $789, %eax
    pushl %eax
    call printi
    movl %eax, %edi
    addl $4, %esp
    movl $L8, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    movl $789, %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call dec2bin
    movl %eax, %edi
    addl $8, %esp
    movl $L9, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    movl $567, %eax
    pushl %eax
    call printi
    movl %eax, %edi
    addl $4, %esp
    movl $L10, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    movl $567, %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call dec2bin
    movl %eax, %edi
    addl $8, %esp
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

dec2bin:
    pushl %ebp
    movl %esp, %ebp
    subl $12, %esp
    movl %ebx, -12(%ebp)
    movl %esi, -4(%ebp)
    movl %edi, -8(%ebp)
    movl 12(%ebp), %ebx
    movl $0, %eax
    cmp %eax, %ebx
    jg L1
L2:
    movl $0, %eax
    movl -12(%ebp), %ebx
    movl -4(%ebp), %esi
    movl -8(%ebp), %edi
    jmp L14
L1:
    movl 12(%ebp), %ebx
    movl $2, %ecx
    movl %ebx, %eax
    cltd
    idivl %ecx
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call dec2bin
    movl %eax, %ebx
    addl $8, %esp
    movl 12(%ebp), %ecx
    movl 12(%ebp), %ebx
    movl $2, %edi
    movl %ebx, %eax
    cltd
    idivl %edi
    imull $2, %eax
    subl %eax, %ecx
    pushl %ecx
    call printi
    movl %eax, %ebx
    addl $4, %esp
    jmp L2
L14:
    movl %ebp, %esp
    popl %ebp
    ret

.section .rodata
L11:
.string "   \n"
L10:
.string "   \t->\t"
L9:
.string "   \n"
L8:
.string "   \t->\t"
L7:
.string "   \n"
L6:
.string "   \t->\t"
L5:
.string "   \n"
L4:
.string "   \t->\t"
