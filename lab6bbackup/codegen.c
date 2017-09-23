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
                                String_fmt("    movl `s1, %d(`s0)\n", n), 
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
                                String_fmt("    movl `s1, %d(`s0)\n", n),
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
                            String_fmt("    movl `s0, %d\n", n),
                            NULL,
                            Temp_TempList(t2, NULL)
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
                            String_fmt("    movl `s1, (`s0)\n"),
                            NULL,
                            Temp_TempList(t1, Temp_TempList(t2, NULL))
                        )
                    );
                }
            }
            else if (dst->kind == T_TEMP && src->kind == T_CONST) {
                /* MOVE(TEMP i, CONST) */
                Temp_temp t1 = munchExp(dst);
                emit(
                    AS_Move(
                        String_fmt("    movl $%d, `d0\n", src->u.CONST),
                        Temp_TempList(t1, NULL),
                        NULL
                    )
                );
            }
            else if (dst->kind == T_TEMP) {
                /* MOVE(TEMP i, e2) */
                Temp_temp t1 = dst->u.TEMP;
                Temp_temp t2 = munchExp(src);
                emit(
                    AS_Move(
                        String_fmt("    movl `s0, `d0\n"),
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
                    String_fmt("    jmp `j0\n"),
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
                    String_fmt("    cmp `s1, `s0\n"),
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
                    jinstr = "jg";
                    break;
                case T_ge:
                    jinstr = "jge";
                    break;
                default:
                    assert(0);
            }
            emit(
                AS_Oper(
                    String_fmt("    %s `j0\n", jinstr),
                    // Temp_TempList(Temp_newtemp(), NULL), //how?
                    NULL,
                    NULL,
                    AS_Targets(Temp_LabelList(s->u.CJUMP.true, NULL))
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
                            String_fmt("    movl %d(`s0), `d0\n", n),
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
                            String_fmt("    movl %d(`s0), `d0\n", n),
                            Temp_TempList(r, NULL),
                            Temp_TempList(t2, NULL),
                            NULL
                        )
                    );
                    return r;
                }
                else if (ee->u.BINOP.left->kind == T_TEMP) {
                    /* MEM(BINOP(PLUS, TEMP, e2)) */
                    Temp_temp t1 = ee->u.BINOP.left->u.TEMP;
                    Temp_temp t2 = munchExp(ee->u.BINOP.right);
                    Temp_temp r = Temp_newtemp();
                    emit(
                        AS_Oper(
                            String_fmt("    addl `s0, `d0\n"),
                            Temp_TempList(t2, NULL),
                            Temp_TempList(t1, Temp_TempList(t2, NULL)),
                            NULL
                        )
                    );
                    emit(
                        AS_Move(
                            String_fmt("    movl (`s0), `d0\n"),
                            Temp_TempList(r, NULL),
                            Temp_TempList(t2, NULL)
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
                            String_fmt("    addl `s0, `d0\n"),
                            Temp_TempList(t1, NULL),
                            Temp_TempList(t2, Temp_TempList(t1, NULL)),
                            NULL
                        )
                    );
                    emit(
                        AS_Move(
                            String_fmt("    movl (`s0), `d0\n"),
                            Temp_TempList(r, NULL),
                            Temp_TempList(t1, NULL)
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
                        String_fmt("    movl %d, `d0\n", n),
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
                        String_fmt("    movl (`s0), `d0\n"),
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
                    opinstr = "subl";
                    break;
                case T_mul:
                    opinstr = "imull";
                    break;
                case T_div:
                    opinstr = "idivl";
                    break;
                default:
                    assert(0);
            }
            if (e->u.BINOP.op==T_plus || e->u.BINOP.op==T_minus || e->u.BINOP.op==T_mul) {
                if (e->u.BINOP.left->kind == T_CONST) {
                    /* BINOP(*, CONST(i), e2) */
                    Temp_temp r = Temp_newtemp();
                    T_exp e2 = e->u.BINOP.right;
                    Temp_temp t2 = munchExp(e2);
                    int n = e->u.BINOP.left->u.CONST;
                    emit(
                        AS_Move(
                            String_fmt("    movl `s0, `d0\n"),
                            Temp_TempList(r, NULL),
                            Temp_TempList(t2, NULL)
                        )
                    );
                    emit(
                        AS_Oper(
                            String_fmt("    %s $%d, `d0\n", opinstr, n),
                            Temp_TempList(r, NULL),
                            Temp_TempList(r, NULL),
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
                            String_fmt("    movl `s0, `d0\n"),
                            Temp_TempList(r, NULL),
                            Temp_TempList(t1, NULL)
                        )
                    );
                    emit(
                        AS_Oper(
                            String_fmt("    %s $%d, `d0\n", opinstr, n),
                            Temp_TempList(r, NULL),
                            Temp_TempList(r, NULL),
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
                            String_fmt("    movl `s0, `d0\n"),
                            Temp_TempList(r, NULL),
                            Temp_TempList(t1, NULL)
                        )
                    );
                    emit(
                        AS_Oper(
                            String_fmt("    %s `s0, `d0\n", opinstr),
                            Temp_TempList(r, NULL),
                            Temp_TempList(t2, Temp_TempList(r, NULL)),
                            NULL
                        )
                    );
                    return r;
                }
            }
            else if (e->u.BINOP.op==T_div) {
                if (e->u.BINOP.left->kind == T_CONST) {
                    /* BINOP(*, CONST(i), e2) */
                    T_exp e2 = e->u.BINOP.right;
                    Temp_temp t2 = munchExp(e2);
                    int n = e->u.BINOP.left->u.CONST;
                    emit(
                        AS_Move(
                            String_fmt(""),
                            Temp_TempList(get_eax(), Temp_TempList(get_edx(), NULL)),
                            NULL
                        )
                    );
                    emit(
                        AS_Move(
                            String_fmt("    movl $%d, `d0\n", n),
                            Temp_TempList(get_eax(), NULL),
                            NULL
                        )
                    );
                    emit(
                        AS_Oper(
                            String_fmt("    cltd\n"),
                            Temp_TempList(get_edx(), NULL),
                            NULL,
                            NULL
                        )
                    );
                    emit(
                        AS_Oper(
                            String_fmt("    idivl `s0\n"),
                            Temp_TempList(get_eax(), Temp_TempList(get_edx(), NULL)),
                            Temp_TempList(t2, NULL),
                            NULL
                        )
                    );
                    emit(
                        AS_Move(
                            String_fmt(""),
                            NULL,
                            Temp_TempList(get_eax(), Temp_TempList(get_edx(), NULL))
                        )
                    );
                    return get_eax();
                }
                else if (e->u.BINOP.right->kind == T_CONST) {
                    /* BINOP(*, e1, CONST(i)) */
                    Temp_temp tmp = Temp_newtemp();
                    T_exp e1 = e->u.BINOP.left;
                    Temp_temp t1 = munchExp(e1);
                    int n = e->u.BINOP.right->u.CONST;
                    emit(
                        AS_Move(
                            String_fmt(""),
                            Temp_TempList(get_eax(), Temp_TempList(get_edx(), NULL)),
                            NULL
                        )
                    );
                    emit(
                        AS_Move(
                            String_fmt("    movl $%d, `d0\n", n),
                            Temp_TempList(tmp, NULL),
                            NULL
                        )
                    );
                    emit(
                        AS_Move(
                            String_fmt("    movl `s0, `d0\n", n),
                            Temp_TempList(get_eax(), NULL),
                            Temp_TempList(t1, NULL)
                        )
                    );
                    emit(
                        AS_Oper(
                            String_fmt("    cltd\n"),
                            Temp_TempList(get_edx(), NULL),
                            NULL,
                            NULL
                        )
                    );
                    emit(
                        AS_Oper(
                            String_fmt("    idivl `s0\n", n),
                            Temp_TempList(get_eax(), Temp_TempList(get_edx(), NULL)),
                            Temp_TempList(tmp, NULL),
                            NULL
                        )
                    );
                    emit(
                        AS_Move(
                            String_fmt(""),
                            NULL,
                            Temp_TempList(get_eax(), Temp_TempList(get_edx(), NULL))
                        )
                    );
                    return get_eax();
                }
                else {
                    /* BINOP(*, e1, e2) */
                    T_exp e1 = e->u.BINOP.left;
                    T_exp e2 = e->u.BINOP.right;
                    Temp_temp t1 = munchExp(e1);
                    Temp_temp t2 = munchExp(e2);
                    emit(
                        AS_Move(
                            String_fmt(""),
                            Temp_TempList(get_eax(), Temp_TempList(get_edx(), NULL)),
                            NULL
                        )
                    );
                    emit(
                        AS_Move(
                            String_fmt("    movl `s0, `d0\n"),
                            Temp_TempList(get_eax(), NULL),
                            Temp_TempList(t1, NULL)
                        )
                    );
                    emit(
                        AS_Oper(
                            String_fmt("    cltd\n"),
                            Temp_TempList(get_edx(), NULL),
                            NULL,
                            NULL
                        )
                    );
                    emit(
                        AS_Oper(
                            String_fmt("    idivl `s0\n"),
                            Temp_TempList(get_eax(), Temp_TempList(get_edx(), NULL)),
                            Temp_TempList(t2, NULL),
                            NULL
                        )
                    );
                    emit(
                        AS_Move(
                            String_fmt(""),
                            NULL,
                            Temp_TempList(get_eax(), Temp_TempList(get_edx(), NULL))
                        )
                    );
                    return get_eax();
                }
            }
            assert(0);
        }
        case T_CONST: {
            /* CONST */
            Temp_temp r = Temp_newtemp();
            int n = e->u.CONST;
            emit(
                AS_Oper(
                    String_fmt("    movl $%d, `d0\n", n),
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
                    String_fmt("    movl $%s, `d0\n", Temp_labelstring(e->u.NAME)),
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
            int cnt = 0;
            ptr = T_reverseExpList(ptr);
            for (; ptr!=NULL; ptr=ptr->tail) {
                tl = Temp_TempList(munchExp(ptr->head), tl);
                emit(
                    AS_Oper(
                        String_fmt("    pushl `s0\n"),
                        NULL,
                        Temp_TempList(tl->head, NULL),
                        NULL
                    )
                );
                cnt++;
            }
            // emit(
            //     AS_Oper(
            //         String_fmt("    call `j0\n"),
            //         get_callersaves(),
            //         NULL,
            //         AS_Targets(Temp_LabelList(e->u.CALL.fun->u.NAME, NULL))
            //     )
            // );
            emit(
                AS_Oper(
                    String_fmt("    call %s\n", S_name(e->u.CALL.fun->u.NAME)),
                    get_callersaves(),
                    NULL,
                    NULL
                )
            );
            emit(
                AS_Oper(
                    String_fmt("    movl %%eax, `d0\n"),
                    Temp_TempList(r, NULL),
                    NULL,
                    NULL
                )
            );
            emit(
                AS_Oper(
                    String_fmt(""),
                    NULL,
                    get_callersaves(),
                    NULL
                )
            );
            emit(
                AS_Oper(
                    String_fmt("    addl $%d, %%esp\n"),
                    NULL,
                    NULL,
                    NULL
                )
            );
            return r;
        }
        default: {
            assert(0);
        }
    }
}

AS_instrList prologue(F_frame f, AS_instrList instr_list) {
	assert(instr_list && instr_list->head->kind==T_LABEL);
    AS_instr instr0 = instr_list->head;
    instr_list = instr_list->tail;
    AS_instr instr1 = AS_Oper(
							String_fmt("    pushl `s0\n"), 
							NULL, 
							Temp_TempList(get_ebp(), NULL), 
							NULL);
    AS_instr instr2 = AS_Move(
							String_fmt("    movl `s0, `d0\n"), 
							Temp_TempList(get_ebp(), NULL), 
							Temp_TempList(get_esp(), NULL));
    AS_instr instr3 = AS_Oper(
							String_fmt("    subl $%d, `s0\n", f->nformals * F_wordSize), 
							NULL, 
							Temp_TempList(get_esp(), NULL), 
							NULL);
    if (f->nformals != 0) {
		return AS_InstrList(instr0,
			   AS_InstrList(instr1,
			   AS_InstrList(instr2,
			   AS_InstrList(instr3, instr_list))));
    }
    else
    {
		return AS_InstrList(instr0,
			   AS_InstrList(instr1,
			   AS_InstrList(instr2, instr_list)));
    }
}

AS_instrList epilogue(F_frame f, AS_instrList instr_list) {
    assert(instr_list && instr_list->head->kind==I_LABEL);
    AS_instr instr0 = AS_Oper(
						"", 
						NULL, 
						get_calleesaves(),
						NULL);
    AS_instr instr1 = AS_Move(
						String_fmt("    movl `s0, `d0\n"), 
						Temp_TempList(get_esp(), NULL), 
						Temp_TempList(get_ebp(), NULL));
    AS_instr instr2 = AS_Oper(
						String_fmt("    popl `d0\n"), 
						Temp_TempList(get_ebp(), NULL), 
						NULL, 
						NULL);
    AS_instr instr3 = AS_Oper(
						String_fmt("    ret\n"), 
                        get_color_regs(), //?
						NULL, 
						NULL);

	return AS_splice(
			instr_list,
			AS_InstrList(instr0,
			AS_InstrList(instr1,
			AS_InstrList(instr2,
			AS_InstrList(instr3, NULL)))));
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
    // ret = prologue(f, ret);
    // return epilogue(f, ret);
}

