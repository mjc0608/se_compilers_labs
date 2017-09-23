#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "table.h"
#include "symbol.h"
#include "absyn.h"
#include "temp.h"
#include "tree.h"
#include "printtree.h"
#include "frame.h"
#include "translate.h"

F_fragList frags = NULL;

Tr_expList Tr_ExpList(Tr_exp head, Tr_expList tail) {
	Tr_expList list = malloc(sizeof(*list));
	list->head = head;
	list->tail = tail;
	return list;
}

static Tr_exp Tr_Ex(T_exp ex) {
	Tr_exp te = malloc(sizeof(*te));
	te->kind = Tr_ex;
	te->u.ex = ex;
	return te;
}

static Tr_exp Tr_Nx(T_stm nx) {
	Tr_exp te = malloc(sizeof(*te));
	te->kind = Tr_nx;
	te->u.nx = nx;
	return te;
}

static Tr_exp Tr_Cx(patchList trues, patchList falses, T_stm stm) {
	Tr_exp te = malloc(sizeof(*te));
	te->kind = Tr_cx;
	te->u.cx.trues = trues;
	te->u.cx.falses = falses;
	te->u.cx.stm = stm;
	return te;	
}

static T_exp unEx(Tr_exp e) {
	switch(e->kind) {
		case Tr_ex:
			return e->u.ex;
		case Tr_cx: {
			Temp_temp r = Temp_newtemp();
			Temp_label t = Temp_newlabel(), f = Temp_newlabel();
			doPatch(e->u.cx.trues, t);
			doPatch(e->u.cx.falses, f);
			return T_Eseq(T_Move(T_Temp(r), T_Const(1)),
					T_Eseq(e->u.cx.stm,
					 T_Eseq(T_Label(f),
					  T_Eseq(T_Move(T_Temp(r), T_Const(0)),
					   T_Eseq(T_Label(t), T_Temp(r))))));
		}
		case Tr_nx:
			return T_Eseq(e->u.nx, T_Const(0));
	}
	assert(0);
}

static T_stm unNx(Tr_exp e) {
	switch(e->kind) {
		case Tr_ex:
			return T_Exp(e->u.ex);
		case Tr_nx:
			return e->u.nx;
		case Tr_cx:
			return T_Exp(unEx(e));
	}
	assert(0);
}

static patchList PatchList(Temp_label *head, patchList tail) {
    patchList list = malloc(sizeof(*list));
    list->head = head;
    list->tail = tail;
    return list;
}

void doPatch(patchList tList, Temp_label label) {
    for(; tList; tList=tList->tail) 
        *(tList->head) = label;
}

patchList joinPatch(patchList first, patchList second) {
	if (!first) return second;
	for (; first->tail; first=first->tail);
	first->tail = second;
	return first;
}

static struct Cx unCx(Tr_exp e) {
	switch(e->kind) {
		case Tr_cx:
			return e->u.cx;
		case Tr_ex: {
#if 0
			struct Cx cx;
			cx.trues = NULL;
			cx.falses = NULL;
			cx.stm = T_Exp(e->u.ex);
			return cx;
#else 
			struct Cx cx;
			cx.stm = T_Cjump(T_ne, e->u.ex, T_Const(0), NULL, NULL);
			cx.trues = PatchList(&(cx.stm->u.CJUMP.true), NULL);
			cx.falses = PatchList(&(cx.stm->u.CJUMP.false), NULL);
			return cx;
#endif
		}
		case Tr_nx:
			break;
	}
	assert(0);
}

Tr_level Tr_outermost(void) {
	static Tr_level outermost = NULL;
	if (!outermost)
		outermost = Tr_newLevel(NULL, Temp_newlabel(), NULL);
	return outermost;
}

Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals) {
	Tr_level level = malloc(sizeof(*level));
	level->parent = parent;
	level->frame = F_newFrame(name, U_BoolList(TRUE, formals)); /* add static link */
	return level;
}

Tr_accessList Tr_formals(Tr_level level) {
	return Tr_AccessList(level); /* the first of the formals is static link */
}

Tr_access Tr_allocLocal(Tr_level level, bool escape) {
	F_frame frame = level->frame;
	Tr_access ta = malloc(sizeof(*ta));
	ta->level = level;
	ta->access = F_allocLocal(frame, escape);
	return ta;
}

Tr_access Tr_Access(Tr_level level, F_access access) {
	if (level == NULL || access == NULL) 
		return NULL;
	Tr_access ta = malloc(sizeof(*ta));
	ta->level = level;
	ta->access = access;
	return ta;
}

Tr_accessList Tr_AccessList(Tr_level level) {
	F_accessList formals;
	Tr_accessList list, ptr;

	if (level==NULL) return NULL;
	formals = F_formals(level->frame);
	formals = formals->tail; /* skip static link */

	list = malloc(sizeof(*list));
	list->head = NULL;
	list->tail = NULL;
	ptr = list;

	while (formals) {
		ptr->head = Tr_Access(level, formals->head);
		if (formals->tail) 
			ptr->tail = malloc(sizeof(*ptr));
		else 
			ptr->tail = NULL;
		formals = formals->tail;
		ptr = ptr->tail;
	}
	return list;
}

Tr_exp Tr_SimpleVar(Tr_access tr_acc, Tr_level level) {
#if 0
	T_exp addr = T_Temp(F_FP());
	
	while (access->level!=level && level!=NULL) { // ???
		F_access a = F_formals(level->frame)->head; //get static link
		addr = F_Exp(a, addr);
		level = level->parent;
	}
	return Tr_Ex(F_Exp(access->access, addr));
#else
    F_access access = tr_acc->access;
    T_exp exp;
    if (level != tr_acc->level) {
		int cnt = 0;
		exp = F_Exp(F_staticLink(), T_Temp(F_FP()));
		level = level->parent;
		while (level != tr_acc->level) {
			exp = F_Exp(F_staticLink(), exp);
			level = level->parent;
		}
		exp = F_Exp(access, exp);
	}
	else {
		exp = F_Exp(access, T_Temp(F_FP()));
    }
    return Tr_Ex(exp);
#endif
}

Tr_exp Tr_SubscriptVar(Tr_exp base, Tr_exp index) {
	return Tr_Ex(T_Mem(T_Binop(
						T_plus, 
						unEx(base),
						T_Binop(T_mul, unEx(index), T_Const(F_wordSize)))));
}

Tr_exp Tr_FieldVar(Tr_exp base, int offset) {
	return Tr_Ex(T_Mem(T_Binop(T_plus, unEx(base), T_Const(offset*F_wordSize))));
}

Tr_exp Tr_OpExp(A_oper op, Tr_exp left, Tr_exp right) {
	T_binOp t_op;
	switch (op) {
		case A_plusOp: t_op = T_plus; break;
		case A_minusOp: t_op = T_minus; break;
		case A_timesOp: t_op = T_mul; break;
		case A_divideOp: t_op = T_div; break;
		default: assert(0);
	}
	return Tr_Ex(T_Binop(t_op, unEx(left), unEx(right)));
}

Tr_exp Tr_IntExp(int val) {
	return Tr_Ex(T_Const(val));
}

Tr_exp Tr_CallExp(Temp_label name, Tr_level fun_level, Tr_level call_level, Tr_expList l, bool needslk) {
#if 0
	T_exp static_link = malloc(sizeof(*static_link));
	T_expList list_h = NULL, list = NULL;

	l = Tr_ExpList(Tr_StaticLink(call_level, fun_level), l);

	for(; l; l=l->tail) {
		T_exp t = unEx(l->head);
		if (!list_h) {
			list_h = T_ExpList(t, NULL);
			list = list_h;
		}
		else {
			list->tail = T_ExpList(t, NULL);
			list = list->tail;
		}
	}

	return Tr_Ex(T_Call(T_Name(name), list_h));
#else
	T_exp staticlink;
	if (fun_level != call_level) {
		staticlink = F_Exp(F_staticLink(), T_Temp(F_FP()));
		call_level = call_level->parent;

		while (fun_level != call_level) {
			staticlink = F_Exp(F_staticLink(), staticlink);
			call_level = call_level->parent;
		}
	}
	else {
		staticlink = T_Temp(F_FP());
	}

	T_expList list_h = NULL;
	T_expList list = NULL;
	for(; l; l=l->tail) {
		T_exp t = unEx(l->head);
		if (!list_h) {
			list_h = T_ExpList(t, NULL);
			list = list_h;
		}
		else {
			list->tail = T_ExpList(t, NULL);
			list = list->tail;
		}
	}

	if (needslk) {
		list_h = T_ExpList(staticlink, list_h);
	}

	return Tr_Ex(T_Call(T_Name(name), list_h));

#endif
}

Tr_exp Tr_StaticLink(Tr_level curr, Tr_level origin) {
#if 1
	T_exp addr = T_Temp(F_FP());
	while (curr && (curr!=origin)) {
		F_access a = F_formals(curr->frame)->head;
		addr = F_Exp(a, addr);
		curr = curr->parent;
	}
	return Tr_Ex(addr);
#else
	
#endif
}

Tr_exp Tr_NoExp() {
	return Tr_Ex(T_Const(0));
}

Tr_exp Tr_NilExp() {
	return Tr_Ex(T_Const(0));
}

Tr_exp Tr_IfExp(Tr_exp test, Tr_exp then, Tr_exp elsee) {
	Tr_exp e = NULL;
	Temp_label t = Temp_newlabel(), f = Temp_newlabel(), leave = Temp_newlabel();
	struct Cx cx = unCx(test);
	doPatch(cx.trues, t);
	doPatch(cx.falses, f);
	if (elsee==NULL) {
		if (then->kind==Tr_nx) {
			e = Tr_Nx(T_Seq(cx.stm, T_Seq(T_Label(t), T_Seq(then->u.nx, T_Label(f)))));
		}
		else if (then->kind==Tr_cx) {
			e = Tr_Nx(T_Seq(cx.stm, T_Seq(T_Label(t), T_Seq(then->u.cx.stm, T_Label(f)))));
		}
		else {
			e = Tr_Nx(T_Seq(cx.stm, T_Seq(T_Label(t), T_Seq(T_Exp(then->u.ex), T_Label(f)))));
		}
	}
	else if (then->kind == Tr_ex || elsee->kind == Tr_ex) {
		Temp_temp r = Temp_newtemp();
		T_stm leave_jump = T_Jump(T_Name(leave), Temp_LabelList(leave, NULL));

		e = Tr_Ex(T_Eseq(cx.stm,
				  T_Eseq(T_Label(t),
				  T_Eseq(T_Move(T_Temp(r), unEx(then)),
				  T_Eseq(leave_jump,
				  T_Eseq(T_Label(f),
				  T_Eseq(T_Move(T_Temp(r), unEx(elsee)),
				  T_Eseq(leave_jump,
				  T_Eseq(T_Label(leave), T_Temp(r))))))))));
	}
	else {
		Temp_temp r = Temp_newtemp();
		T_stm leave_jump = T_Jump(T_Name(leave), Temp_LabelList(leave, NULL));
		T_stm then_stm, else_stm;
		switch(then->kind) {
			case Tr_ex:
				then_stm = T_Exp(then->u.ex);
				break;
			case Tr_nx:
				then_stm = then->u.nx;
				break;
			case Tr_cx:
				then_stm = then->u.cx.stm;
		}
		switch(elsee->kind) {
			case Tr_ex:
				else_stm = T_Exp(elsee->u.ex);
				break;
			case Tr_nx:
				else_stm = elsee->u.nx;
				break;
			case Tr_cx:
				else_stm = elsee->u.cx.stm;
		}

		e = Tr_Nx(T_Seq(cx.stm,
				  T_Seq(T_Label(t), 
				  T_Seq(then_stm, 
				  T_Seq(leave_jump, 
			      T_Seq(T_Label(f), 
				  T_Seq(else_stm, 
				  T_Seq(leave_jump, NULL))))))));
	}

	return e;
}

Tr_exp Tr_WhileExp(Tr_exp test, Tr_exp body, Tr_exp done) {
#if 0
	Temp_label test_label = Temp_newlabel(), body_label = Temp_newlabel();
	return Tr_Nx(T_Seq(T_Jump(T_Name(test_label), Temp_LabelList(test_label, NULL)),
				 T_Seq(T_Label(body_label),
				 T_Seq(unNx(body),
				 T_Seq(T_Label(test_label),
				 T_Seq(T_Cjump(T_eq, unEx(test), T_Const(0), unEx(done)->u.NAME, body_label),
				 T_Label(unEx(done)->u.NAME)))))));
#else 
	Temp_label start_label = Temp_newlabel(), t = Temp_newlabel(), f = unEx(done)->u.NAME;
	struct Cx cx_test = unCx(test);

	doPatch(cx_test.trues, t);
	doPatch(cx_test.falses, f);

	return Tr_Nx(T_Seq(T_Label(start_label), 
				 T_Seq(cx_test.stm, 
				 T_Seq(T_Label(t), 
				 T_Seq(unNx(body), 
				 T_Seq(T_Jump(T_Name(start_label), Temp_LabelList(start_label, NULL)), 
				 T_Label(f)))))));
#endif
}

Tr_exp Tr_ForExp(Tr_exp lo, Tr_exp hi, Tr_exp body, Temp_label done) {
	assert(0);
	//should modify the AST of for to let-while
}

static F_fragList str_frags = NULL;
Tr_exp Tr_StringExp(string str, Tr_level level) {
	Temp_label str_label = Temp_newlabel();
	F_frag fragment = F_StringFrag(str_label, str);
	str_frags = F_FragList(fragment, str_frags);
	return Tr_Ex(T_Name(str_label));
}

Tr_exp Tr_RecordExp(int n, Tr_expList fields) {
	Temp_temp rec = Temp_newtemp();
	T_stm alloc = T_Move(T_Temp(rec), F_ExternalCall("allocRecord",T_ExpList(T_Const(F_wordSize*n), NULL)));
	int i = n-1;

	T_stm seq = T_Move(T_Mem(T_Binop(T_plus, T_Temp(rec), T_Const(i*F_wordSize))), unEx(fields->head));
	i--;

	for (fields=fields->tail; fields!=NULL; fields=fields->tail, i--) {
		seq = T_Seq(T_Move(T_Mem(T_Binop(T_plus, T_Temp(rec), T_Const(i*F_wordSize))), unEx(fields->head)),seq);
	}

	return Tr_Ex(T_Eseq(T_Seq(alloc, seq), T_Temp(rec)));
}

Tr_exp Tr_ArrayExp(Tr_exp size, Tr_exp init) {
	return Tr_Ex(F_ExternalCall(String("initArray"), T_ExpList(unEx(size), T_ExpList(unEx(init), NULL))));
}

Tr_exp Tr_AssignExp(Tr_exp lvalue, Tr_exp rvalue) {
	return Tr_Nx(T_Move(unEx(lvalue), unEx(rvalue)));
}

// Tr_exp Tr_FunDec(Tr_level level, Tr_exp body, Tr_accessList accessList) {
// 	T_stm ts = unEx(body);
// 	frags = F_FragList(F_ProcFrag(ts, level->frame), frags);
// 	return Tr_Ex(NULL);
// }

void Tr_procEntryExit(Tr_level level, Tr_exp body, Tr_accessList formals, Temp_label label) {
	T_stm mod_body = F_procEntryExit1(unNx(body));
	// T_stm mod_body = unNx(body);
	T_stm s = T_Seq(T_Label(label), mod_body);
	F_frag pf = F_ProcFrag(s, level->frame);
	frags = F_FragList(pf, frags);
}

// Tr_exp Tr_funDec(Tr_level level, Tr_exp body) {
// 	T_stm ts = unNx(body);
// 	frags = F_FragList(F_ProcFrag(ts, level->frame));
// 	return 
// }

Tr_expList Tr_expList_append(Tr_expList list, Tr_exp exp) {
	Tr_expList p = list;
	Tr_expList newList = Tr_ExpList(exp, NULL);
	
	if (list==NULL) {
		return newList;
	}

	while (list->tail!=NULL) {
		list=list->tail;
	}

	list->tail = newList;
	
	return p;
}

Tr_expList Tr_reverse_expList(Tr_expList list) {
	Tr_expList p = list;
	Tr_expList last = NULL;

	for (; p; p=p->tail) {
		last = Tr_ExpList(p->head, last);
	}
	return last;
}

Tr_exp Tr_CmpExp(A_oper op, Tr_exp left, Tr_exp right) {
	T_relOp opp;
	switch(op) {
		case A_eqOp: opp = T_eq; break;
		case A_neqOp: opp = T_ne; break;
		case A_ltOp: opp = T_lt; break;
		case A_leOp: opp = T_le; break;
		case A_gtOp: opp = T_gt; break;
		case A_geOp: opp = T_ge; break;
		default: assert(0);
	}

	T_stm cond = T_Cjump(opp, unEx(left), unEx(right), NULL, NULL);
	patchList trues = PatchList(&cond->u.CJUMP.true, NULL);
	patchList falses = PatchList(&cond->u.CJUMP.false, NULL);

	return Tr_Cx(trues, falses, cond);
}

Tr_exp Tr_EqStringExp(A_oper op, Tr_exp left, Tr_exp right) {
	T_exp r = F_ExternalCall(String("stringEqual"), T_ExpList(unEx(left), T_ExpList(unEx(right), NULL)));
	if (op == A_eqOp) return Tr_Ex(r);
	else if (op == A_neqOp) {
		T_exp e = (r->u.CONST!=0)?T_Const(0):T_Const(1);
		return Tr_Ex(e);
	}
	assert(0);
}

Tr_exp Tr_SeqExp(Tr_expList l) {
	Tr_expList l_reverse = Tr_reverse_expList(l);
	T_exp r = unEx(l_reverse->head);
	for (l_reverse=l_reverse->tail; l_reverse; l_reverse=l_reverse->tail) {
		r = T_Eseq(T_Exp(unEx(l_reverse->head)), r);
	}
	return Tr_Ex(r);
}

Tr_exp Tr_DoneExp() {
	return Tr_Ex(T_Name(Temp_newlabel()));
}

Tr_exp Tr_BreakExp(Tr_exp b) {
	return Tr_Nx(T_Jump(T_Name(unEx(b)->u.NAME), 
						Temp_LabelList(unEx(b)->u.NAME, NULL)));
}

F_fragList Tr_getResult() {
	F_fragList cur = NULL, prev = NULL;
	for (cur = str_frags; cur; cur = cur->tail)
		prev = cur;
	if (prev) prev->tail = frags;
	return str_frags ? str_frags : frags;
}