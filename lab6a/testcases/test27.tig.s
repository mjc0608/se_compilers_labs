BEGIN function
L32:
movl $0, 101
movl 101, -8(100)
movl g, 103
movl -4(100), 104
movl $2, 105
call 103
movl L31, 106
jmp 106
L31:

END function

BEGIN function
L34:
movl -8(100), 107
movl L33, 108
jmp 108
L33:

END function

