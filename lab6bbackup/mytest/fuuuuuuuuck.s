 SEQ(
  LABEL tigermain,
  SEQ(
   MOVE(
    TEMP t101,
    TEMP t107),
   SEQ(
    MOVE(
     TEMP t102,
     TEMP t106),
    SEQ(
     MOVE(
      TEMP t103,
      TEMP t105),
     SEQ(
      MOVE(
       TEMP t104,
       ESEQ(
        EXP(
         ESEQ(
          MOVE(
           MEM(
            BINOP(PLUS,
             TEMP t100,
             CONST -4)),
           CALL(
            NAME getchar)),
          CONST 0)),
        ESEQ(
         SEQ(
          CJUMP(NE,
           CALL(
            NAME stringEqual,
             MEM(
              BINOP(PLUS,
               TEMP t100,
               CONST -4)),
             NAME L1),
           CONST 0,
           L2,L3),
          SEQ(
           LABEL L2,
           SEQ(
            EXP(
             CALL(
              NAME printi,
               CONST 111)),
            LABEL L3))),
         CONST 0))),
      SEQ(
       MOVE(
        TEMP t107,
        TEMP t101),
       SEQ(
        MOVE(
         TEMP t106,
         TEMP t102),
        SEQ(
         MOVE(
          TEMP t105,
          TEMP t103),
         MOVE(
          TEMP t104,
          TEMP t104)))))))))
 LABEL tigermain
 MOVE(
  TEMP t101,
  TEMP t107)
 MOVE(
  TEMP t102,
  TEMP t106)
 MOVE(
  TEMP t103,
  TEMP t105)
 MOVE(
  TEMP t113,
  BINOP(PLUS,
   TEMP t100,
   CONST -4))
 MOVE(
  TEMP t112,
  CALL(
   NAME getchar))
 MOVE(
  MEM(
   TEMP t113),
  TEMP t112)
 MOVE(
  TEMP t111,
  CALL(
   NAME stringEqual,
    MEM(
     BINOP(PLUS,
      TEMP t100,
      CONST -4)),
    NAME L1))
 CJUMP(NE,
  TEMP t111,
  CONST 0,
  L2,L3)
 LABEL L3
 MOVE(
  TEMP t104,
  CONST 0)
 MOVE(
  TEMP t107,
  TEMP t101)
 MOVE(
  TEMP t106,
  TEMP t102)
 MOVE(
  TEMP t105,
  TEMP t103)
 MOVE(
  TEMP t104,
  TEMP t104)
 JUMP(
  NAME L5)
 LABEL L2
 EXP(
  CALL(
   NAME printi,
    CONST 111))
 JUMP(
  NAME L3)
 LABEL L5
t106 -> %esi
t100 -> %ebp
t105 -> %edi
t107 -> %ebx
t108 -> %ecx
t104 -> %eax
t109 -> %edx
t101  spilled (0): 20 19 18 17 16 15 14 13 12 10 11 9 8 7 6 5 3 2 1 
t106  %esi p  (1): 10 6 0 
t100  %ebp p  (2): 14 13 12 10 11 9 8 7 6 5 0 
t105  %edi p  (3): 10 5 0 
t107  %ebx p  (4): 6 10 5 
t102  %esi    (5): 20 19 4 18 17 16 15 14 13 12 10 11 9 8 7 6 2 0 3 
t103  spilled (6): 20 19 1 4 18 17 16 15 14 13 12 10 11 9 8 7 0 5 2 
t114  %eax    (7): 2 5 6 0 
t113  %edi    (8): 13 12 10 11 9 2 5 6 0 
t108  %ecx p  (9): 20 16 12 11 10 8 0 6 5 2 
t104  %eax p  (10): 20 3 1 4 16 12 8 0 6 5 2 11 9 
t109  %edx p  (11): 20 16 12 10 8 0 6 5 2 9 
t115  %ebx    (12): 2 5 6 0 8 10 11 9 
t112  %eax    (13): 2 5 6 0 8 
t117  %eax    (14): 2 5 6 0 
t118  %eax    (15): 5 6 0 
t116  %ebx    (16): 0 6 5 10 11 9 
t111  %ebx    (17): 18 0 6 5 
t119  %eax    (18): 5 6 0 17 
t121  %eax    (19): 0 6 5 
t120  %ebx    (20): 5 6 0 10 11 9 
tigermain:
    movl %ebx, 101
    movl %esi, 102
    movl %edi, 103
    movl %ebp, 114
    addl $-4, 114
    movl 114, 113
    call getchar
    movl %eax, 115
    movl 115, 112
    movl 112, (113)
    movl $L1, 117
    pushl 117
    movl -4(%ebp), 118
    pushl 118
    call stringEqual
    movl %eax, 116
    movl 116, 111
    movl $0, 119
    cmp 119, 111
    jne L2
L3:
    movl $0, %eax
    movl 101, %ebx
    movl 102, %esi
    movl 103, %edi
    movl %eax, %eax
    jmp L5
L2:
    movl $111, 121
    pushl 121
    call printi
    movl %eax, 120
    jmp L3
L5:

t100 -> %ebp
t105 -> %edi
t106 -> %esi
t107 -> %ebx
t108 -> %ecx
t104 -> %eax
t109 -> %edx
t122  %eax    (0): 3 2 1 
t100  %ebp p  (1): 22 21 3 4 19 18 17 16 15 14 13 12 10 11 9 8 7 6 5 0 
t105  %edi p  (2): 10 5 0 
t106  %esi p  (3): 10 1 0 
t107  %ebx p  (4): 1 10 5 
t102  %esi    (5): 22 21 4 19 18 17 16 15 14 13 12 10 11 9 8 7 6 1 2 
t123  %eax    (6): 5 1 
t114  %eax    (7): 1 5 
t113  %edi    (8): 13 12 10 11 9 1 5 
t108  %ecx p  (9): 22 16 12 11 10 8 5 1 
t104  %eax p  (10): 22 2 20 3 4 19 16 12 8 5 1 11 9 
t109  %edx p  (11): 22 16 12 10 8 5 1 9 
t115  %ebx    (12): 1 5 8 10 11 9 
t112  %eax    (13): 1 5 8 
t117  %eax    (14): 1 5 
t118  %eax    (15): 5 1 
t116  %ebx    (16): 1 5 10 11 9 
t111  %ebx    (17): 18 1 5 
t119  %eax    (18): 5 1 17 
t124  %ebx    (19): 5 10 1 
t125  %ebx    (20): 10 
t121  %eax    (21): 1 5 
t120  %ebx    (22): 5 1 10 11 9 
