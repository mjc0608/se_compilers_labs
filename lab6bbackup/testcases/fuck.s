.text
.globl tigermain
.type tigermain, @function

tigermain:
    pushl %ebp
    movl %esp, %ebp
    subl $24, %esp
    movl %ebx, %eax
    movl %eax, -16(%ebp)    #swap t143 out with t170
    movl %esi, %eax
    movl %eax, -20(%ebp)    #swap t144 out with t171
    movl %edi, %eax
    movl %eax, -24(%ebp)    #swap t145 out with t172
    movl %ebp, %eax
    addl $-4, %eax
    movl %eax, %ebx
    call getchar
    movl %eax, %edi
    movl %edi, %eax
    movl %eax, (%ebx)
    movl %ebp, %eax
    addl $-8, %eax
    movl %eax, %ebx
    pushl %ebp
    call readlist
    movl %eax, %edi
    movl %edi, %eax
    movl %eax, (%ebx)
    movl %ebp, %eax
    addl $-12, %eax
    movl %eax, %ebx
    movl %ebp, %eax
    addl $-4, %eax
    movl %eax, %edi
    call getchar
    movl %eax, %esi
    movl %esi, %eax
    movl %eax, (%edi)
    pushl %ebp
    call readlist
    movl %eax, %edi
    movl %edi, (%ebx)
    movl %ebp, %ebx
    movl -12(%ebp), %eax
    pushl %eax
    movl -8(%ebp), %eax
    pushl %eax
    pushl %ebp
    call merge
    movl %eax, %edi
    movl %edi, %eax
    pushl %eax
    pushl %ebx
    call printlist
    movl %eax, %ebx
    movl %ebx, %eax
    movl -16(%ebp), %ebx    #swap t143 in with t173
    movl %ebx, %ebx
    movl -20(%ebp), %ebx    #swap t144 in with t174
    movl %ebx, %esi
    movl -24(%ebp), %ebx    #swap t145 in with t175
    movl %ebx, %edi
    movl %eax, %eax
    jmp L49
L49:
    movl %ebp, %esp
    popl %ebp
    ret

printlist:
    pushl %ebp
    movl %esp, %ebp
    subl $4, %esp
    movl %ebx, %ebx
    movl %esi, %esi
    movl %edi, %eax
    movl %eax, -4(%ebp)    #swap t142 out with t190
    movl 12(%ebp), %ecx
    movl $0, %eax
    cmp %eax, %ecx
    je L46
L47:
    movl 12(%ebp), %eax
    movl 0(%eax), %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call printint
    movl %eax, %edi
    movl $L45, %eax
    pushl %eax
    call print
    movl %eax, %edi
    movl 12(%ebp), %eax
    movl 4(%eax), %eax
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call printlist
    movl %eax, %edi
    movl %edi, %eax
L48:
    movl %eax, %eax
    movl %ebx, %ebx
    movl %esi, %esi
    movl -4(%ebp), %ebx    #swap t142 in with t191
    movl %ebx, %edi
    movl %eax, %eax
    jmp L50
L46:
    movl $L44, %eax
    pushl %eax
    call print
    movl %eax, %edi
    movl %edi, %eax
    jmp L48
L50:
    movl %ebp, %esp
    popl %ebp
    ret

printint:
    pushl %ebp
    movl %esp, %ebp
    subl $4, %esp
    movl %ebx, %ebx
    movl %esi, %esi
    movl %edi, %eax
    movl %eax, -4(%ebp)    #swap t138 out with t205
    movl 12(%ebp), %ecx
    movl $0, %eax
    cmp %eax, %ecx
    jl L41
L42:
    movl 12(%ebp), %ecx
    movl $0, %eax
    cmp %eax, %ecx
    jg L38
L39:
    movl $L37, %eax
    pushl %eax
    call print
    movl %eax, %edi
    movl %edi, %eax
L40:
    movl %eax, %eax
L43:
    movl %eax, %eax
    movl %ebx, %ebx
    movl %esi, %esi
    movl -4(%ebp), %ebx    #swap t138 in with t206
    movl %ebx, %edi
    movl %eax, %eax
    jmp L51
L41:
    movl $L36, %eax
    pushl %eax
    call print
    movl %eax, %edi
    movl 12(%ebp), %eax
    movl %eax, %eax
    subl $0, %eax
    pushl %eax
    pushl %ebp
    call f
    movl %eax, %edi
    movl %edi, %eax
    jmp L43
L38:
    movl 12(%ebp), %eax
    pushl %eax
    pushl %ebp
    call f
    movl %eax, %edi
    movl %edi, %eax
    jmp L40
L51:
    movl %ebp, %esp
    popl %ebp
    ret

f:
    pushl %ebp
    movl %esp, %ebp
    subl $12, %esp
    movl %ebx, %eax
    movl %eax, -12(%ebp)    #swap t131 out with t230
    movl %esi, %eax
    movl %eax, -4(%ebp)    #swap t132 out with t226
    movl %edi, %eax
    movl %eax, -8(%ebp)    #swap t133 out with t227
    movl 12(%ebp), %ebx
    movl $0, %eax
    cmp %eax, %ebx
    jg L33
L34:
    movl -12(%ebp), %eax    #swap t131 in with t231
    movl %eax, %ebx
    movl -4(%ebp), %eax    #swap t132 in with t228
    movl %eax, %esi
    movl -8(%ebp), %eax    #swap t133 in with t229
    movl %eax, %edi
    jmp L52
L33:
    movl 12(%ebp), %ecx
    movl $10, %ebx
    movl %ecx, %eax
    cltd
    idivl %ebx
    pushl %eax
    movl 8(%ebp), %eax
    pushl %eax
    call f
    movl %eax, %ebx
    movl 12(%ebp), %ecx
    movl 12(%ebp), %edi
    movl $10, %ebx
    movl %edi, %eax
    cltd
    idivl %ebx
    movl %eax, %ebx
    imull $10, %ebx
    movl %ecx, %eax
    subl %ebx, %eax
    movl %eax, %edi
    movl $L32, %eax
    pushl %eax
    call ord
    movl %eax, %ebx
    movl %ebx, %ebx
    movl %edi, %eax
    addl %ebx, %eax
    pushl %eax
    call chr
    movl %eax, %ebx
    movl %ebx, %eax
    pushl %eax
    call print
    movl %eax, %ebx
    jmp L34
L52:
    movl %ebp, %esp
    popl %ebp
    ret

merge:
    pushl %ebp
    movl %esp, %ebp
    subl $12, %esp
    movl %ebx, %eax
    movl %eax, -4(%ebp)    #swap t128 out with t266
    movl %esi, %eax
    movl %eax, -8(%ebp)    #swap t129 out with t267
    movl %edi, %eax
    movl %eax, -12(%ebp)    #swap t130 out with t268
    movl 12(%ebp), %ebx			# a
    movl $0, %eax				# 0
    cmp %eax, %ebx				# a=nil
    je L29
L30:
    movl 16(%ebp), %ebx			# b
    movl $0, %eax				# 0
    cmp %eax, %ebx				# b=nil
    je L26
L27:
    movl 12(%ebp), %eax			# a
    movl 0(%eax), %ebx			# a.first
    movl 16(%ebp), %eax			# b
    movl 0(%eax), %eax			# b.first
    cmp %eax, %ebx				# a.first < b.first
    jl L23
L24:
    movl $8, %eax				# 8
    pushl %eax					#
    call allocRecord			#
    movl %eax, %ebx				#
    movl %ebx, %ebx				# list
    movl 16(%ebp), %eax			# b
    movl 0(%eax), %eax			# b.first
    movl %eax, 0(%ebx)			# list.first = b.first
    movl %ebx, %eax				# list
    addl $4, %eax				# list.rest
    movl %eax, %edi				# list.rest
    movl 16(%ebp), %eax			# b
    movl 4(%eax), %eax			# b.rest
    pushl %eax
    movl 12(%ebp), %eax			# a
    pushl %eax
    movl 8(%ebp), %eax			# staticlink
    pushl %eax
    call merge
    movl %eax, %esi				# ret
    movl %esi, %eax				# ret
    movl %eax, (%edi)			# list.rest = ret
    movl %ebx, %eax				# list
L25:
    movl %eax, %eax				# list
L28:
    movl %eax, %eax				# list
L31:
    movl %eax, %eax				# eax = b
    movl -4(%ebp), %ebx    #swap t128 in with t269
    movl %ebx, %ebx			# restore ebx
    movl -8(%ebp), %ebx    #swap t129 in with t270
    movl %ebx, %esi			# restore esi
    movl -12(%ebp), %ebx    #swap t130 in with t271 
    movl %ebx, %edi			# restore edi
    movl %eax, %eax
    jmp L53
L29:
    movl 16(%ebp), %eax			# b
    movl %eax, %eax				# b
    jmp L31
L26:
    movl 12(%ebp), %eax			# a
    movl %eax, %eax
    jmp L28
L23:
    movl $8, %eax				# 8
    pushl %eax					#
    call allocRecord			# allocrecord 8
    movl %eax, %ebx				# list
    movl %ebx, %ebx				# list
    movl 12(%ebp), %eax			# a
    movl 0(%eax), %eax			# a.first
    movl %eax, 0(%ebx)			# list.first = a.fisrt
    movl %ebx, %eax				# list
    addl $4, %eax				# list.rest
    movl %eax, %edi				# list.rest
    movl 16(%ebp), %eax			# b
    pushl %eax					#
    movl 12(%ebp), %eax			# a
    movl 4(%eax), %eax			# a.rest
    pushl %eax					#
    movl 8(%ebp), %eax			# staticlink
    pushl %eax					#
    call merge					#
    movl %eax, %esi				# ret
    movl %esi, %eax				# ret
    movl %eax, (%edi)			# list.rest = ret
    movl %ebx, %eax
    jmp L25
L53:
    movl %ebp, %esp
    popl %ebp
    ret

readlist:
    pushl %ebp
    movl %esp, %ebp
    subl $20, %esp
    movl %ebx, %eax
    movl %eax, -12(%ebp)    #swap t120 out with t294
    movl %esi, %eax
    movl %eax, -16(%ebp)    #swap t121 out with t295
    movl %edi, %eax
    movl %eax, -20(%ebp)    #swap t122 out with t296
    movl %ebp, %eax				#
    addl $-4, %eax				# any
    movl %eax, %ebx				# any
    movl $4, %eax				# 4
    pushl %eax
    call allocRecord
    movl %eax, %edi				# any
    movl %edi, %ecx				# any
    movl $0, %eax				# 0
    movl %eax, 0(%ecx)			# any.any = 0
    movl %ecx, (%ebx)			# (any) = any
    movl %ebp, %eax				#
    addl $-8, %eax				# i
    movl %eax, %ebx				# i
    movl -4(%ebp), %eax			# any
    pushl %eax					# any
    movl 8(%ebp), %eax			# staticlink
    pushl %eax					#
    call readint
    movl %eax, %edi				# ret
    movl %edi, %eax				# ret
    movl %eax, (%ebx)			# i = ret
    movl -4(%ebp), %eax			# any
    movl 0(%eax), %ebx			# any.any
    movl $0, %eax				# 0
    cmp %eax, %ebx				#
    jne L20
L21:
    movl $0, %eax
L22:
    movl %eax, %eax
    movl -12(%ebp), %ebx    #swap t120 in with t297
    movl %ebx, %ebx
    movl -16(%ebp), %ebx    #swap t121 in with t298
    movl %ebx, %esi
    movl -20(%ebp), %ebx    #swap t122 in with t299
    movl %ebx, %edi
    movl %eax, %eax
    jmp L54
L20:
    movl $8, %eax				# 8
    pushl %eax					#
    call allocRecord
    movl %eax, %ebx				# list
    movl %ebx, %ebx				# list
    movl -8(%ebp), %eax			# i
    movl %eax, 0(%ebx)			# list.first = i
    movl %ebx, %eax				# list
    addl $4, %eax				# list.rest
    movl %eax, %edi				# list.rest
    movl 8(%ebp), %eax			# staticlink
    pushl %eax
    call readlist
    movl %eax, %esi				# ret
    movl %esi, %eax				# ret
    movl %eax, (%edi)			# list.rest = ret
    movl %ebx, %eax				# list -> eax
    jmp L22
L54:
    movl %ebp, %esp
    popl %ebp
    ret

readint:
    pushl %ebp
    movl %esp, %ebp
    subl $16, %esp
    movl %ebx, %eax
    movl %eax, -8(%ebp)    #swap t114 out with t335
    movl %esi, %eax
    movl %eax, -12(%ebp)    #swap t115 out with t336
    movl %edi, %eax
    movl %eax, -16(%ebp)    #swap t116 out with t337
    movl $0, %eax				# 0
    movl %eax, -4(%ebp)			# i = 0
    pushl %ebp
    call skipto
    movl %eax, %ebx
    movl 12(%ebp), %eax			# any
    movl %eax, %eax				# any
    addl $0, %eax				# any.any
    movl %eax, %ebx				# any.any
    movl 8(%ebp), %eax			# staticlink
    movl -4(%eax), %eax			# buffer
    pushl %eax
    pushl %ebp
    call isdigit
    movl %eax, %edi				# ret
    movl %edi, %eax				# ret
    movl %eax, (%ebx)			# any.any = isdigit(buffer)
L18:
    movl 8(%ebp), %eax			# staticlink
    movl -4(%eax), %eax			# buffer
    pushl %eax
    pushl %ebp
    call isdigit
    movl %eax, %ebx				# ret
    movl %ebx, %ebx				# ret
    movl $0, %eax				# 0
    cmp %eax, %ebx
    jne L19
L16:
    movl -4(%ebp), %eax
    movl %eax, %eax
    movl -8(%ebp), %ebx    #swap t114 in with t338
    movl %ebx, %ebx
    movl -12(%ebp), %ebx    #swap t115 in with t339
    movl %ebx, %esi
    movl -16(%ebp), %ebx    #swap t116 in with t340
    movl %ebx, %edi
    movl %eax, %eax
    jmp L55
L19:
    movl %ebp, %eax				#
    addl $-4, %eax				#
    movl %eax, %esi				# addr of i
    movl -4(%ebp), %eax			# i
    movl %eax, %eax				# i
    imull $10, %eax				# i*10
    movl %eax, %edi				# i*10
    movl 8(%ebp), %eax			# staticlink
    movl -4(%eax), %eax			# buffer
    pushl %eax
    call ord
    movl %eax, %ebx				# ret
    movl %ebx, %ebx				# ret
    movl %edi, %eax				# i*10
    addl %ebx, %eax				# ret + i*10
    movl %eax, %edi				# ret + i*10
    movl $L17, %eax				# "0"
    pushl %eax
    call ord
    movl %eax, %ebx				# ret2
    movl %ebx, %ebx				# ret2
    movl %edi, %eax				# ret + i*10
    subl %ebx, %eax				# ret + i*10 - ret2
    movl %eax, (%esi)			# i = ret + i*10 -ret2
    movl 8(%ebp), %eax			# staticlink
    movl %eax, %eax				# staticlink
    addl $-4, %eax				# staticlink - 4
    movl %eax, %ebx				# staticlink - 4
    call getchar
    movl %eax, %edi				# ret
    movl %edi, %eax				# ret
    movl %eax, (%ebx)			# buffer = ret
    jmp L18
L55:
    movl %ebp, %esp
    popl %ebp
    ret

skipto:
    pushl %ebp
    movl %esp, %ebp
    subl $8, %esp
    movl %ebx, %eax
    movl %eax, -4(%ebp)    #swap t111 out with t360
    movl %esi, %esi
    movl %edi, %eax
    movl %eax, -8(%ebp)    #swap t113 out with t361
L14:
    movl $L8, %eax			# " "
    pushl %eax
    movl 8(%ebp), %eax		# staticlink
    movl 8(%eax), %eax		# staticlink-staticlink
    movl -4(%eax), %eax		# buffer
    pushl %eax				# buffer
    call stringEqual		# call
    movl %eax, %ebx			# result
    movl %ebx, %ebx			# result
    movl $0, %eax			# 0
    cmp %eax, %ebx			# cmp result 0
    jne L10
L11:						# not equal
    movl $L9, %eax			# "\n"
    pushl %eax
    movl 8(%ebp), %eax		# staticlink
    movl 8(%eax), %eax		# staticlink-staticlink
    movl -4(%eax), %eax		# buffer
    pushl %eax
    call stringEqual		# call
    movl %eax, %ebx			# result
    movl %ebx, %ebx
L12:
    movl $0, %eax			# 0
    cmp %eax, %ebx			#
    jne L15
L13:
    movl -4(%ebp), %eax    #swap t111 in with t362
    movl %eax, %ebx
    movl %esi, %esi
    movl -8(%ebp), %eax    #swap t113 in with t363
    movl %eax, %edi
    jmp L56
L10:
    movl $1, %ebx			# 1
    jmp L12
L15:
    movl 8(%ebp), %eax
    movl 8(%eax), %eax
    movl %eax, %eax			# staticlink-staticlink
    addl $-4, %eax			# buffer
    movl %eax, %edi			#
    call getchar
    movl %eax, %ebx			# result
    movl %ebx, %eax			# result
    movl %eax, (%edi)		# buffer = result
    jmp L14
L56:
    movl %ebp, %esp
    popl %ebp
    ret

isdigit:
    pushl %ebp
    movl %esp, %ebp
    subl $12, %esp
    movl %ebx, %eax
    movl %eax, -4(%ebp)    #swap t103 out with t382
    movl %esi, %eax
    movl %eax, -8(%ebp)    #swap t104 out with t383
    movl %edi, %eax
    movl %eax, -12(%ebp)    #swap t105 out with t384
    movl 8(%ebp), %eax			# staticlink
    movl 8(%eax), %eax			# staticlink-staticlink
    movl -4(%eax), %eax			# buffer
    pushl %eax					#
    call ord					#
    movl %eax, %ebx				# ret1
    movl %ebx, %eax				# ret1
    movl %eax, %ebx				# ret1
    movl $L1, %eax				# "0"
    pushl %eax					#
    call ord					#
    movl %eax, %edi				# ret2
    movl %edi, %eax				# ret2
    cmp %eax, %ebx
    jge L3
L4:
    movl $0, %eax				# eax
L5:
    movl %eax, %eax
    movl -4(%ebp), %ebx    #swap t103 in with t385
    movl %ebx, %ebx
    movl -8(%ebp), %ebx    #swap t104 in with t386
    movl %ebx, %esi
    movl -12(%ebp), %ebx    #swap t105 in with t387
    movl %ebx, %edi
    movl %eax, %eax
    jmp L57
L3:
    movl $1, %ebx				# 1
    movl 8(%ebp), %eax			# staticlink
    movl 8(%eax), %eax			# staticlink-staticlink
    movl -4(%eax), %eax			# buffer
    pushl %eax
    call ord
    movl %eax, %edi				# ret1
    movl %edi, %eax				# ret1
    movl %eax, %edi				# ret1
    movl $L2, %eax				# "9"
    pushl %eax
    call ord
    movl %eax, %esi				# ret2
    movl %esi, %eax				# ret2
    cmp %eax, %edi
    jle L6
L7:
    movl $0, %ebx
L6:
    movl %ebx, %eax				# eax
    jmp L5
L57:
    movl %ebp, %esp
    popl %ebp
    ret

.section .rodata
L45:
.string "    "
L44:
.string "   \n"
L37:
.string "   0"
L36:
.string "   -"
L32:
.string "   0"
L17:
.string "   0"
L9:
.string "   \n"
L8:
.string "    "
L2:
.string "   9"
L1:
.string "   0"
