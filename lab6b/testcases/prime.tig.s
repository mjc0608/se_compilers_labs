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
    jmp L18
L18:
    movl %ebp, %esp
    popl %ebp
    ret

try:
    pushl %ebp
    movl %esp, %ebp
    subl $4, %esp
    movl %edi, -4(%ebp)
    movl $56, %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call check
    movl %eax, %edi
    addl $8, %esp
    pushl %edi
    call printi
    movl %eax, %edi
    addl $4, %esp
    movl $L7, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    movl $23, %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call check
    movl %eax, %edi
    addl $8, %esp
    pushl %edi
    call printi
    movl %eax, %edi
    addl $4, %esp
    movl $L8, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    movl $71, %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call check
    movl %eax, %edi
    addl $8, %esp
    pushl %edi
    call printi
    movl %eax, %edi
    addl $4, %esp
    movl $L9, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    movl $72, %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call check
    movl %eax, %edi
    addl $8, %esp
    pushl %edi
    call printi
    movl %eax, %edi
    addl $4, %esp
    movl $L10, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    movl $173, %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call check
    movl %eax, %edi
    addl $8, %esp
    pushl %edi
    call printi
    movl %eax, %edi
    addl $4, %esp
    movl $L11, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    movl $181, %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call check
    movl %eax, %edi
    addl $8, %esp
    pushl %edi
    call printi
    movl %eax, %edi
    addl $4, %esp
    movl $L12, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    movl $281, %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call check
    movl %eax, %edi
    addl $8, %esp
    pushl %edi
    call printi
    movl %eax, %edi
    addl $4, %esp
    movl $L13, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    movl $659, %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call check
    movl %eax, %edi
    addl $8, %esp
    pushl %edi
    call printi
    movl %eax, %edi
    addl $4, %esp
    movl $L14, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    movl $729, %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call check
    movl %eax, %edi
    addl $8, %esp
    pushl %edi
    call printi
    movl %eax, %edi
    addl $4, %esp
    movl $L15, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    movl $947, %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call check
    movl %eax, %edi
    addl $8, %esp
    pushl %edi
    call printi
    movl %eax, %edi
    addl $4, %esp
    movl $L16, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    movl $945, %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call check
    movl %eax, %edi
    addl $8, %esp
    pushl %edi
    call printi
    movl %eax, %edi
    addl $4, %esp
    movl $L17, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    movl %edi, %eax
    movl -4(%ebp), %edi
    jmp L19
L19:
    movl %ebp, %esp
    popl %ebp
    ret

check:
    pushl %ebp
    movl %esp, %ebp
    subl $12, %esp
    movl %ebx, -4(%ebp)
    movl %esi, -8(%ebp)
    movl %edi, -12(%ebp)
    movl $1, %edi
    movl $2, %esi
    movl 12(%ebp), %ebx
    movl $2, %ecx
    movl %ebx, %eax
    cltd
    idivl %ecx
    movl %eax, %ecx
L5:
    cmp %ecx, %esi
    jle L6
L1:
    movl %edi, %eax
    movl -4(%ebp), %ebx
    movl -8(%ebp), %esi
    movl -12(%ebp), %edi
    jmp L20
L6:
    movl 12(%ebp), %ebx
    movl %ebx, %eax
    cltd
    idivl %esi
    imull %esi, %eax
    movl 12(%ebp), %ebx
    cmp %ebx, %eax
    je L2
L3:
    addl $1, %esi
    jmp L5
L2:
    movl $0, %edi
    jmp L1
L20:
    movl %ebp, %esp
    popl %ebp
    ret

.section .rodata
L17:
.string "   \n"
L16:
.string "   \n"
L15:
.string "   \n"
L14:
.string "   \n"
L13:
.string "   \n"
L12:
.string "   \n"
L11:
.string "   \n"
L10:
.string "   \n"
L9:
.string "   \n"
L8:
.string "   \n"
L7:
.string "   \n"
