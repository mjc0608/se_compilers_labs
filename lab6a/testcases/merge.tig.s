 


0
-
0
0


 
9
0
BEGIN function
L124:
movl 100, 125
addl $-8, 125
movl 125, 124
movl L3_getchar, 127
call 127
movl 126, 123
movl 123, (124)
movl 100, 128
addl $-12, 128
movl 128, 122
movl L78_readlist, 130
movl -4(100), 131
call 130
movl 129, 121
movl 121, (122)
movl 100, 132
addl $-16, 132
movl 132, 120
movl 100, 133
addl $-8, 133
movl 133, 119
movl L3_getchar, 135
call 135
movl 134, 118
movl 118, (119)
movl L78_readlist, 137
movl -4(100), 138
call 137
movl 136, (120)
movl -4(100), 139
movl 139, 117
movl L79_merge, 141
movl -4(100), 142
movl -12(100), 143
movl -16(100), 144
call 141
movl 140, 116
movl L81_printlist, 146
call 146
movl L123, 147
jmp 147
L123:

END function

BEGIN function
L126:
movl -8(100), 148
cmp 148, 106
je 149
L121:
movl L80_printint, 151
movl -4(100), 153
movl -4(153), 152
movl -8(100), 155
movl 0(155), 154
call 151
movl L1_print, 157
movl -4(100), 158
movl L119, 159
call 157
movl L81_printlist, 161
movl -8(100), 163
movl 1(163), 162
call 161
movl 160, 115
L122:
movl L125, 164
jmp 164
L120:
movl L1_print, 166
movl -4(100), 167
movl L118, 168
call 166
movl 165, 115
movl L122, 169
jmp 169
L125:

END function

BEGIN function
L128:
movl -8(100), 170
movl $0, 171
cmp 170, 171
jl 172
L116:
movl -8(100), 173
movl $0, 174
cmp 173, 174
jgt 175
L113:
movl L1_print, 177
movl -4(100), 178
movl L111, 179
call 177
movl 176, 113
L114:
movl 113, 114
L117:
movl L127, 180
jmp 180
L115:
movl L1_print, 182
movl -4(100), 183
movl L110, 184
call 182
movl L105_f, 186
movl -4(100), 188
movl -4(188), 187
movl -8(100), 190
movl 190, 189
sub $0, 189
call 186
movl 185, 114
movl L117, 191
jmp 191
L112:
movl L105_f, 193
movl -4(100), 195
movl -4(195), 194
movl -8(100), 196
call 193
movl 192, 113
movl L114, 197
jmp 197
L127:

END function

BEGIN function
L130:
movl -8(100), 203
movl $0, 204
cmp 203, 204
jgt 205
L108:
movl L129, 206
jmp 206
L107:
movl L105_f, 208
movl -8(100), 210
movl 210, 209
div $10, 209
call 208
movl -4(100), 212
movl -4(212), 211
movl 211, 202
movl -4(100), 214
movl -4(214), 213
movl 213, 201
movl -8(100), 216
movl -8(100), 219
movl 219, 218
div $10, 218
movl 218, 217
mul $10, 217
movl 216, 215
sub 217, 215
movl 215, 200
movl L4_ord, 221
movl -4(100), 223
movl -4(223), 222
movl L106, 224
call 221
movl 220, 199
movl L5_chr, 226
movl 200, 227
addl 199, 227
call 226
movl 225, 198
movl L1_print, 229
call 229
movl L108, 230
jmp 230
L129:

END function

BEGIN function
L132:
movl -8(100), 235
cmp 235, 106
je 236
L92:
movl -12(100), 237
cmp 237, 106
je 238
L89:
movl -8(100), 240
movl 0(240), 239
movl -12(100), 242
movl 0(242), 241
cmp 239, 241
jl 243
L86:
movl initRecord, 245
movl $8, 246
call 245
movl 244, 109
movl 109, 247
addl $0, 247
movl 247, 232
movl L79_merge, 249
movl -8(100), 250
movl -12(100), 252
movl 1(252), 251
call 249
movl 248, 231
movl 231, (232)
movl -12(100), 254
movl 0(254), 253
movl 253, 4(109)
movl 109, 110
L87:
movl 110, 111
L90:
movl 111, 112
L93:
movl L131, 255
jmp 255
L91:
movl -12(100), 256
movl 256, 112
movl L93, 257
jmp 257
L88:
movl -8(100), 258
movl 258, 111
movl L90, 259
jmp 259
L85:
movl initRecord, 261
movl $8, 262
call 261
movl 260, 108
movl 108, 263
addl $0, 263
movl 263, 234
movl L79_merge, 265
movl -8(100), 267
movl 1(267), 266
movl -12(100), 268
call 265
movl 264, 233
movl 233, (234)
movl -8(100), 270
movl 0(270), 269
movl 269, 4(108)
movl 108, 110
movl L87, 271
jmp 271
L131:

END function

BEGIN function
L134:
movl 100, 277
addl $-8, 277
movl 277, 276
movl initRecord, 279
movl $4, 280
call 279
movl 278, 104
movl $0, 281
movl 281, 0(104)
movl 104, (276)
movl 100, 282
addl $-12, 282
movl 282, 275
movl L34_readint, 284
movl -4(100), 286
movl -4(286), 285
movl -8(100), 287
call 284
movl 283, 274
movl 274, (275)
movl -8(100), 289
movl 0(289), 288
L82:
movl initRecord, 291
movl $8, 292
call 291
movl 290, 105
movl 105, 293
addl $0, 293
movl 293, 273
movl L78_readlist, 295
call 295
movl 294, 272
movl 272, (273)
movl -12(100), 296
movl 296, 4(105)
movl 105, 107
L84:
movl L133, 297
jmp 297
L83:
movl 106, 107
movl L84, 298
jmp 298
L133:

END function

BEGIN function
L136:
movl $0, 309
movl 309, -12(100)
movl L47_skipto, 311
movl -4(100), 313
movl -4(313), 312
call 311
movl -8(100), 315
movl 315, 314
addl $0, 314
movl 314, 308
movl L46_isdigit, 317
movl -4(100), 319
movl -4(319), 318
movl -4(100), 321
movl -8(321), 320
call 317
movl 316, 307
movl 307, (308)
movl L65, 322
jmp 322
L66:
movl 100, 323
addl $-12, 323
movl 323, 305
movl -12(100), 325
movl 325, 324
mul $10, 324
movl 324, 302
movl L4_ord, 327
movl -4(100), 328
movl -4(100), 330
movl -8(330), 329
call 327
movl 326, 301
movl 302, 331
addl 301, 331
movl 331, 304
movl L4_ord, 333
movl -4(100), 334
movl L64, 335
call 333
movl 332, 303
movl 304, 336
sub 303, 336
movl 336, (305)
movl -4(100), 338
movl 338, 337
addl $-8, 337
movl 337, 300
movl L3_getchar, 340
movl -4(100), 341
call 340
movl 339, 299
movl 299, (300)
movl L46_isdigit, 343
movl -4(100), 345
movl -4(345), 344
movl -4(100), 347
movl -8(347), 346
call 343
movl 342, 306
movl $0, 348
cmp 306, 348
jne 349
L63:
movl -12(100), 350
movl L135, 351
jmp 351
L135:

END function

BEGIN function
L138:
movl L61, 354
jmp 354
L62:
movl -4(100), 357
movl -4(357), 356
movl 356, 355
addl $-8, 355
movl 355, 353
movl L3_getchar, 359
movl -4(100), 361
movl -4(361), 360
call 359
movl 358, 352
movl 352, (353)
movl stringEqual, 363
movl -4(100), 366
movl -4(366), 365
movl -8(365), 364
movl L55, 367
call 363
L57:
movl $1, 368
movl 368, 103
L59:
movl $0, 369
cmp 103, 369
jne 370
L60:
movl L137, 371
jmp 371
L58:
movl stringEqual, 373
movl -4(100), 376
movl -4(376), 375
movl -8(375), 374
movl L56, 377
call 373
movl 372, 103
movl L59, 378
jmp 378
L137:

END function

BEGIN function
L140:
movl L4_ord, 386
movl -4(100), 388
movl -4(388), 387
movl -4(100), 391
movl -4(391), 390
movl -8(390), 389
call 386
movl 385, 382
movl 382, 384
movl L4_ord, 393
movl -4(100), 395
movl -4(395), 394
movl L48, 396
call 393
movl 392, 383
cmp 384, 383
jge 397
L51:
movl $0, 398
movl 398, 101
L52:
movl L139, 399
jmp 399
L50:
movl $1, 400
movl 400, 102
movl L4_ord, 402
movl -4(100), 404
movl -4(404), 403
movl -4(100), 407
movl -4(407), 406
movl -8(406), 405
call 402
movl 401, 379
movl 379, 381
movl L4_ord, 409
movl -4(100), 411
movl -4(411), 410
movl L49, 412
call 409
movl 408, 380
cmp 381, 380
jle 413
L54:
movl $0, 414
movl 414, 102
L53:
movl 102, 101
movl L52, 415
jmp 415
L139:

END function

