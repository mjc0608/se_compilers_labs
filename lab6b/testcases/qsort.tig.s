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
    call dosort
    movl %eax, %ebx
    addl $4, %esp
    movl %ebx, %eax
    movl -12(%ebp), %ebx
    movl -16(%ebp), %edi
    jmp L32
L32:
    movl %ebp, %esp
    popl %ebp
    ret

dosort:
    pushl %ebp
    movl %esp, %ebp
    subl $12, %esp
    movl %ebx, -4(%ebp)
    movl %esi, -8(%ebp)
    movl %edi, -12(%ebp)
    movl 8(%ebp), %eax
    pushl %eax
    call init
    movl %eax, %ebx
    addl $4, %esp
    movl 8(%ebp), %eax
    movl -4(%eax), %eax
    subl $1, %eax
    pushl %eax
    movl $0, %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call quicksort
    movl %eax, %ebx
    addl $12, %esp
    movl $0, %esi
    movl 8(%ebp), %eax
    movl -4(%eax), %ebx
    subl $1, %ebx
L29:
    cmp %ebx, %esi
    jle L30
L27:
    movl $L31, %eax
    pushl %eax
    call print
    movl %eax, %ebx
    addl $4, %esp
    movl %ebx, %eax
    movl -4(%ebp), %ebx
    movl -8(%ebp), %esi
    movl -12(%ebp), %edi
    jmp L33
L30:
    movl 8(%ebp), %eax
    movl -8(%eax), %ecx
    movl %esi, %eax
    imull $4, %eax
    addl %eax, %ecx
    movl (%ecx), %eax
    pushl %eax
    call printi
    movl %eax, %edi
    addl $4, %esp
    movl $L28, %eax
    pushl %eax
    call print
    movl %eax, %edi
    addl $4, %esp
    addl $1, %esi
    jmp L29
L33:
    movl %ebp, %esp
    popl %ebp
    ret

quicksort:
    pushl %ebp
    movl %esp, %ebp
    subl $12, %esp
    movl %ebx, -4(%ebp)
    movl %esi, -8(%ebp)
    movl %edi, -12(%ebp)
    movl 12(%ebp), %eax
    movl %eax, %esi
    movl 16(%ebp), %eax
    movl %eax, %ecx
    movl 8(%ebp), %eax
    movl -8(%eax), %ebx
    movl 12(%ebp), %eax
    imull $4, %eax
    addl %eax, %ebx
    movl (%ebx), %eax
    movl %eax, %edx
    movl 12(%ebp), %ebx
    movl 16(%ebp), %eax
    cmp %eax, %ebx
    jl L24
L25:
    movl $0, %eax
    movl -4(%ebp), %ebx
    movl -8(%ebp), %esi
    movl -12(%ebp), %edi
    jmp L34
L24:
L22:
    cmp %ecx, %esi
    jl L23
L5:
    movl 8(%ebp), %eax
    movl -8(%eax), %ebx
    movl %esi, %eax
    imull $4, %eax
    addl %eax, %ebx
    movl %edx, (%ebx)
    movl %esi, %eax
    subl $1, %eax
    pushl %eax
    movl 12(%ebp), %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call quicksort
    movl %eax, %ebx
    addl $12, %esp
    movl 16(%ebp), %eax
    pushl %eax
    movl %esi, %eax
    addl $1, %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call quicksort
    movl %eax, %ebx
    addl $12, %esp
    jmp L25
L23:
L12:
    cmp %ecx, %esi
    jl L6
L7:
    movl $0, %ebx
L8:
    movl $0, %eax
    cmp %eax, %ebx
    jne L13
L11:
    movl 8(%ebp), %eax
    movl -8(%eax), %ebx
    movl %esi, %eax
    imull $4, %eax
    addl %eax, %ebx
    movl 8(%ebp), %eax
    movl -8(%eax), %edi
    movl %ecx, %eax
    imull $4, %eax
    addl %eax, %edi
    movl (%edi), %eax
    movl %eax, (%ebx)
L20:
    cmp %ecx, %esi
    jl L14
L15:
    movl $0, %ebx
L16:
    movl $0, %eax
    cmp %eax, %ebx
    jne L21
L19:
    movl 8(%ebp), %eax
    movl -8(%eax), %ebx
    movl %ecx, %eax
    imull $4, %eax
    addl %eax, %ebx
    movl 8(%ebp), %eax
    movl -8(%eax), %edi
    movl %esi, %eax
    imull $4, %eax
    addl %eax, %edi
    movl (%edi), %eax
    movl %eax, (%ebx)
    jmp L22
L6:
    movl $1, %ebx
    movl 8(%ebp), %eax
    movl -8(%eax), %edi
    movl %ecx, %eax
    imull $4, %eax
    addl %eax, %edi
    movl (%edi), %eax
    cmp %eax, %edx
    jle L9
L10:
    movl $0, %ebx
L9:
    jmp L8
L13:
    movl %ecx, %eax
    subl $1, %eax
    movl %eax, %ecx
    jmp L12
L14:
    movl $1, %ebx
    movl 8(%ebp), %eax
    movl -8(%eax), %edi
    movl %esi, %eax
    imull $4, %eax
    addl %eax, %edi
    movl (%edi), %eax
    cmp %eax, %edx
    jge L17
L18:
    movl $0, %ebx
L17:
    jmp L16
L21:
    movl %esi, %eax
    addl $1, %eax
    movl %eax, %esi
    jmp L20
L34:
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
    jmp L35
L4:
    movl 8(%ebp), %eax
    movl -8(%eax), %ecx
    movl %ebx, %eax
    imull $4, %eax
    addl %eax, %ecx
    movl 8(%ebp), %eax
    movl -4(%eax), %eax
    subl %ebx, %eax
    movl %eax, (%ecx)
    movl 8(%ebp), %eax
    pushl %eax
    call nop
    movl %eax, %esi
    addl $4, %esp
    addl $1, %ebx
    jmp L3
L35:
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
    jmp L36
L36:
    movl %ebp, %esp
    popl %ebp
    ret

.section .rodata
L31:
.string "   \n"
L28:
.string "    "
L1:
.string "    "
