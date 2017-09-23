#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "temp.h"
#include "errormsg.h"
#include "tree.h"
#include "assem.h"
#include "frame.h"
#include "codegen.h"
#include "table.h"

static AS_instrList iList = NULL, last = NULL;
static void emit(AS_instr inst) {
    if (last!=NULL)
        last = last->tail = AS_InstrList(inst, NULL);
    else
        last = iList = AS_InstrList(inst, NULL);
}

static Temp_temp munchExp(T_exp e);

static void munchStm(T_stm s) {
    switch(s->kind) {
        case T_MOVE: {
            T_exp dst = s->u.MOVE.dst, src = s->u.MOVE.src;
            if (dst->kind == T_MEM) {
                if (dst->u.MEM->kind == T_BINOP
                    && dst->u.MEM->u.BINOP.op == T_plus
                    && dst->u.MEM->u.BINOP.right->kind == T_CONST) {
                        /* MOVE(MEM(4(PLUS,e1,CONST(i))), e2) */
                        T_exp e1 = dst->u.MEM->u.BINOP.left, e2 = src;
                        int n = dst->u.MEM->u.BINOP.right->u.CONST;
                        Temp_temp t1 = munchExp(e1);
                        Temp_temp t2 = munchExp(e2);

                        emit(
                            AS_Move(
                                String_fmt("movl `s1, %d(`s0)\n", n), 
                                NULL, 
                                Temp_TempList(t1, Temp_TempList(t2, NULL))
                            )
                        );
                }
                else if (dst->u.MEM->kind == T_BINOP
                    && dst->u.MEM->u.BINOP.op == T_plus
                    && dst->u.MEM->u.BINOP.left->kind == T_CONST) {
                        /* MOVE(MEM(BINOP(PLUS,CONST(i),e1)),e2) */
                        T_exp e1 = dst->u.MEM->u.BINOP.right, e2 = src;
                        int n = dst->u.MEM->u.BINOP.left->u.CONST;
                        Temp_temp t1 = munchExp(e1);
                        Temp_temp t2 = munchExp(e2);
                        
                        emit(
                            AS_Move(
                                String_fmt("movl `s1, %d(`s0)\n", n),
                                NULL,
                                Temp_TempList(t1, Temp_TempList(t2, NULL))
                            )
                        );
                }
                else if (dst->u.MEM->kind == T_CONST) {
                    T_exp e2 = src;
                    int n = dst->u.MEM->u.CONST;
                    Temp_temp t2 = munchExp(e2);
                    emit(
                        AS_Move(
                            String_fmt("movl `s0, %d\n", n),
                            NULL,
                            Temp_TempList(t2, NULL)
                        )
                    );
                }
                else if (src->kind == T_MEM) {
                    /* MOVE(MEM(e1), MEM(e2)) */
                    T_exp e1 = dst->u.MEM, e2 = src->u.MEM;
                    int n = e1->u.CONST;
                    Temp_temp t1 = munchExp(e1);
                    Temp_temp t2 = munchExp(e2);
                    emit(
                        AS_Move(
                            String_fmt("movl `s1, %d\n"),
                            NULL,
                            Temp_TempList(t1, Temp_TempList(t2, NULL))
                        )
                    );
                }
                else {
                    /* MOVE(MEM(e1), e2) */
                    T_exp e1 = dst->u.MEM, e2 = src;
                    Temp_temp t1 = munchExp(e1);
                    Temp_temp t2 = munchExp(e2);
                    
                    emit(
                        AS_Move(
                            String_fmt("movl `s1, (`s0)\n"),
                            NULL,
                            Temp_TempList(t1, Temp_TempList(t2, NULL))
                        )
                    );
                }
            }
            else if (dst->kind == T_TEMP) {
                /* MOVE(TEMP i, e2) */
                Temp_temp t1 = munchExp(dst);
                Temp_temp t2 = munchExp(src);
                emit(
                    AS_Move(
                        String_fmt("movl `s0, `d0\n"),
                        Temp_TempList(t1, NULL),
                        Temp_TempList(t2, NULL)
                    )
                );
            }
            else {
                assert(0);
            }
            break;
        }
        case T_SEQ: {
            /* SEQ(stm1, stm2) */
            munchStm(s->u.SEQ.left);
            munchStm(s->u.SEQ.right);
            break;
        }
        case T_LABEL: {
            emit(
                AS_Label(
                    String_fmt("%s:\n", Temp_labelstring(s->u.LABEL)),
                    s->u.LABEL
                )
            );
            break;
        }
        case T_JUMP: {
            /* JUMP(exp) */
            // Temp_temp t = munchExp(s->u.JUMP.exp);
            emit(
                AS_Oper(
                    String_fmt("jmp `j0\n"),
                    // Temp_TempList(t, NULL),
                    NULL,
                    NULL,
                    AS_Targets(s->u.JUMP.jumps)
                )
            );
            break;
        }
        case T_CJUMP: {
            /* CJUMP(op, left, right, trues, falses) */
            Temp_temp left = munchExp(s->u.CJUMP.left);
            Temp_temp right = munchExp(s->u.CJUMP.right);
            string jinstr = NULL;
            emit(
                AS_Oper(
                    String_fmt("cmp `s0, `s1\n"),
                    NULL,
                    Temp_TempList(left, Temp_TempList(right, NULL)),
                    NULL
                )
            );
            switch (s->u.CJUMP.op) {
                case T_eq:
                    jinstr = "je";
                    break;
                case T_ne:
                    jinstr = "jne";
                    break;
                case T_lt:
                    jinstr = "jl";
                    break;
                case T_le:
                    jinstr = "jle";
                    break;
                case T_gt:
                    jinstr = "jgt";
                    break;
                case T_ge:
                    jinstr = "jge";
                    break;
                default:
                    assert(0);
            }
            emit(
                AS_Oper(
                    String_fmt("%s `j0\n", jinstr),
                    // Temp_TempList(Temp_newtemp(), NULL), //how?
                    NULL,
                    NULL,
                    AS_Targets(Temp_LabelList(s->u.CJUMP.true, Temp_LabelList(s->u.CJUMP.false, NULL)))
                )
            );
            break;
        }
        case T_EXP: {
            munchExp(s->u.EXP);
            break;
        }
        default: {
            assert(0);
        }
    }
}

static Temp_temp munchExp(T_exp e) {
    switch(e->kind) {
        case T_MEM: {
            T_exp ee = e->u.MEM;
            if (ee->kind == T_BINOP && ee->u.BINOP.op == T_plus) {
                if (ee->u.BINOP.right->kind == T_CONST) {
                    /* MEM(BINOP(PLUS, e1, CONST)) */
                    Temp_temp r = Temp_newtemp();
                    Temp_temp t1 = munchExp(ee->u.BINOP.left);
                    int n = ee->u.BINOP.right->u.CONST;
                    emit(
                        AS_Oper(
                            String_fmt("movl %d(`s0), `d0\n", n),
                            Temp_TempList(r, NULL),
                            Temp_TempList(t1, NULL),
                            NULL
                        )
                    );
                    return r;
                }
                else if (ee->u.BINOP.left->kind == T_CONST) {
                    /* MEM(BINOP(PLUS, CONST, e2)) */
                    Temp_temp r = Temp_newtemp();
                    Temp_temp t2 = munchExp(ee->u.BINOP.right);
                    int n = ee->u.BINOP.left->u.CONST;
                    emit(
                        AS_Oper(
                            String_fmt("movl %d(`s0), `d0\n", n),
                            Temp_TempList(r, NULL),
                            Temp_TempList(t2, NULL),
                            NULL
                        )
                    );
                    return r;
                }
                else if (ee->u.BINOP.left->kind == T_MEM) {
                    /* MEM(BINOP(PLUS, MEM, e2)) */
                    Temp_temp r = Temp_newtemp();
                    Temp_temp t1 = munchExp(ee->u.BINOP.left);
                    Temp_temp t2 = munchExp(ee->u.BINOP.right);
                    emit(
                        AS_Oper(
                            String_fmt("addl `s0, `d0\n"),
                            Temp_TempList(t1, NULL),
                            Temp_TempList(t2, NULL),
                            NULL
                        )
                    );
                    emit(
                        AS_Oper(
                            String_fmt("movl (`s0), `d0\n"),
                            Temp_TempList(r, NULL),
                            Temp_TempList(t1, NULL),
                            NULL
                        )
                    );
                    return r;
                }
                else {
                    assert(0);// nested memory...
                }
                assert(0);
            }
            else if (ee->kind == T_CONST) {
                /* MEM(CONST) */
                Temp_temp r = Temp_newtemp();
                int n = ee->u.CONST;
                emit(
                    AS_Oper(
                        String_fmt("movl (`s0), `d0\n", n),
                        Temp_TempList(r, NULL),
                        NULL,
                        NULL
                    )
                );
                return r;
            }
            else {
                /* MEM(e1) */
                Temp_temp r = Temp_newtemp();
                Temp_temp t1 = munchExp(ee);
                emit(
                    AS_Oper(
                        String_fmt("movl (`s0), `d0\n"),
                        Temp_TempList(r, NULL),
                        Temp_TempList(t1, NULL),
                        NULL
                    )
                );
                return r;
            }
            assert(0);
        }
        case T_BINOP: {
            string opinstr = NULL;
            switch(e->u.BINOP.op) {
                case T_plus:
                    opinstr = "addl";
                    break;
                case T_minus:
                    opinstr = "sub";
                    break;
                case T_mul:
                    opinstr = "mul";
                    break;
                case T_div:
                    opinstr = "div";
                    break;
                default:
                    assert(0);
            }
            if (e->u.BINOP.left->kind == T_CONST) {
                /* BINOP(*, CONST(i), e2) */
                Temp_temp r = Temp_newtemp();
                T_exp e2 = e->u.BINOP.right;
                Temp_temp t2 = munchExp(e2);
                int n = e->u.BINOP.left->u.CONST;
                emit(
                    AS_Move(
                        String_fmt("movl `s1, `s0\n"),
                        NULL,
                        Temp_TempList(r, Temp_TempList(t2, NULL))
                    )
                );
                emit(
                    AS_Oper(
                        String_fmt("%s $%d, `d0\n", opinstr, n),
                        Temp_TempList(r, NULL),
                        NULL,
                        NULL
                    )
                );
                return r;
            }
            else if (e->u.BINOP.right->kind == T_CONST) {
                /* BINOP(*, e1, CONST(i)) */
                Temp_temp r = Temp_newtemp();
                T_exp e1 = e->u.BINOP.left;
                Temp_temp t1 = munchExp(e1);
                int n = e->u.BINOP.right->u.CONST;
                emit(
                    AS_Move(
                        String_fmt("movl `s1, `s0\n"),
                        NULL,
                        Temp_TempList(r, Temp_TempList(t1, NULL))
                    )
                );
                emit(
                    AS_Oper(
                        String_fmt("%s $%d, `d0\n", opinstr, n),
                        Temp_TempList(r, NULL),
                        NULL,
                        NULL
                    )
                );
                return r;
            }
            else {
                /* BINOP(*, e1, e2) */
                Temp_temp r = Temp_newtemp();
                T_exp e1 = e->u.BINOP.left;
                T_exp e2 = e->u.BINOP.right;
                Temp_temp t1 = munchExp(e1);
                Temp_temp t2 = munchExp(e2);
                emit(
                    AS_Move(
                        String_fmt("movl `s1, `s0\n"),
                        NULL,
                        Temp_TempList(r, Temp_TempList(t1, NULL))
                    )
                );
                emit(
                    AS_Oper(
                        String_fmt("%s `s0, `d0\n", opinstr),
                        Temp_TempList(r, NULL),
                        Temp_TempList(t2, NULL),
                        NULL
                    )
                );
                return r;
            }
            assert(0);
        }
        case T_CONST: {
            /* CONST */
            Temp_temp r = Temp_newtemp();
            int n = e->u.CONST;
            emit(
                AS_Oper(
                    String_fmt("movl $%d, `d0\n", n),
                    Temp_TempList(r, NULL),
                    NULL,
                    NULL
                )
            );
            return r;
        }
        case T_TEMP: {
            return e->u.TEMP;
        }
        case T_ESEQ: {
            munchStm(e->u.ESEQ.stm);
            return munchExp(e->u.ESEQ.exp);
        }
        case T_NAME: {
            Temp_temp r = Temp_newtemp();
            emit(
                AS_Oper(
                    String_fmt("movl %s, `d0\n", Temp_labelstring(e->u.NAME)),
                    Temp_TempList(r, NULL),
                    NULL,
                    NULL
                )
            );
            return r;
        }
        case T_CALL: {
            Temp_temp r = Temp_newtemp();
            // Temp_temp c = munchExp(e->u.CALL.fun);
            Temp_tempList tl = NULL;
            T_expList ptr = e->u.CALL.args;
            for (; ptr!=NULL; ptr=ptr->tail) {
                tl = Temp_TempList(munchExp(ptr->head), tl);
            }
            emit(
                AS_Oper(
                    String_fmt("call `j0\n"),
                    NULL,
                    // Temp_TempList(c, NULL),
                    NULL,
                    AS_Targets(Temp_LabelList(e->u.CALL.fun->u.NAME, NULL))
                )
            );
            return r;
        }
        default: {
            assert(0);
        }
    }
}


//Lab 6: your code here
AS_instrList F_codegen(F_frame f, T_stmList stmList) {
    T_stmList stm_l = stmList;
    for (; stm_l; stm_l = stm_l->tail)
        munchStm(stm_l->head);
    AS_instrList ret = iList;
    iList = NULL;
    last = NULL;
    return ret;
}