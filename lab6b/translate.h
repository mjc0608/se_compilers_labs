#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "frame.h"
#include "temp.h"
#include "absyn.h"

#define FUCK_THE_LAB

/* Lab5: your code below */

typedef struct patchList_ *patchList;
struct patchList_ {
    Temp_label *head;
    patchList tail;
};
void doPatch(patchList tList, Temp_label label);
patchList joinPatch(patchList first, patchList second);

typedef struct Tr_level_ *Tr_level;
typedef struct Tr_access_ *Tr_access;
struct Tr_access_ {
    Tr_level level;
    F_access access;
};
Tr_access Tr_Access(Tr_level level, F_access access);

typedef struct Tr_accessList_ *Tr_accessList;
struct Tr_accessList_ {
	Tr_access head;
	Tr_accessList tail;	
};
Tr_accessList Tr_AccessList(Tr_level level);

struct Tr_level_ {
    Tr_level parent;
    F_frame frame;
};
Tr_level Tr_outermost(void);
Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals);
Tr_accessList Tr_formals(Tr_level level);
Tr_access Tr_allocLocal(Tr_level level, bool escape);

struct Cx {
    patchList trues;
    patchList falses;
    T_stm stm;
};

typedef struct Tr_exp_ *Tr_exp;
struct Tr_exp_ {
	enum {Tr_ex, Tr_nx, Tr_cx} kind;
	union {
		T_exp ex;
		T_stm nx;
		struct Cx cx;
	} u;
};

typedef struct Tr_expList_ *Tr_expList;
struct Tr_expList_ {
    Tr_exp head;
    Tr_expList tail;
};

/* store all frags */
extern F_fragList frags;
F_fragList Tr_getResult();

void Tr_procEntryExit(Tr_level level, Tr_exp body, Tr_accessList formals, Temp_label label);

Tr_expList Tr_ExpList(Tr_exp head, Tr_expList tail);
Tr_expList Tr_reverse_expList(Tr_expList list);

Tr_exp Tr_SimpleVar(Tr_access access, Tr_level level);
Tr_exp Tr_SubscriptVar(Tr_exp base, Tr_exp index);
Tr_exp Tr_FieldVar(Tr_exp base, int offset);

Tr_exp Tr_OpExp(A_oper op, Tr_exp left, Tr_exp right);
Tr_exp Tr_IntExp(int val);
Tr_exp Tr_CallExp(Temp_label name, Tr_level fun_level, Tr_level call_level, Tr_expList l, bool needskl);
Tr_exp Tr_NoExp();
Tr_exp Tr_NilExp();
Tr_exp Tr_IfExp(Tr_exp test, Tr_exp then, Tr_exp elsee);
Tr_exp Tr_WhileExp(Tr_exp test, Tr_exp body, Tr_exp done);
Tr_exp Tr_DoneExp();
Tr_exp Tr_BreakExp(Tr_exp b);
Tr_exp Tr_ForExp(Tr_exp lo, Tr_exp hi, Tr_exp body, Temp_label done);
Tr_exp Tr_StringExp(string str, Tr_level level);
Tr_exp Tr_RecordExp(int n, Tr_expList fields);
Tr_exp Tr_ArrayExp(Tr_exp size, Tr_exp init);
Tr_exp Tr_AssignExp(Tr_exp lvalue, Tr_exp rvalue);
Tr_exp Tr_StaticLink(Tr_level curr, Tr_level origin);
Tr_exp Tr_CmpExp(A_oper op, Tr_exp left, Tr_exp right);
Tr_exp Tr_EqStringExp(A_oper op, Tr_exp left, Tr_exp right);
Tr_exp Tr_SeqExp(Tr_expList l);

Tr_expList Tr_expList_append(Tr_expList list, Tr_exp exp);

#endif
