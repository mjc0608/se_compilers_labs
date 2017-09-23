#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "table.h"
#include "tree.h"
#include "frame.h"

/*Lab5: Your implementation here.*/
#define WORD_SIZE 4
const int F_wordSize = WORD_SIZE;

static F_access InFrame(int offset) {
	F_access fa = malloc(sizeof(*fa));
	fa->kind = inFrame;
	fa->u.offset = offset;
	return fa;
}

static F_access InReg(Temp_temp reg) {
	F_access fa = malloc(sizeof(*fa));
	fa->kind = inReg;
	fa->u.reg = reg;
	return fa;
}

static F_accessList F_AccessList(F_frame frame, U_boolList formals) {
	U_boolList curr=formals;
	F_accessList head = NULL, ptr = NULL;

	if (curr==NULL) return NULL;

	head = malloc(sizeof(*head));
	head->head = F_allocLocal(frame, curr->head);
	head->tail = NULL;
	ptr = head;
	curr = curr->tail;

	while (curr!=NULL) {
		ptr->tail = malloc(sizeof(*ptr));
		ptr = ptr->tail;
		ptr->head = F_allocLocal(frame, curr->head);
		ptr->tail = NULL;
		curr = curr->tail;
	}
	return head;
}

F_access F_allocLocal(F_frame f, bool escape) {
#ifdef ALL_ESCAPE
	f->nformals++;
	F_access access = InFrame(-1*f->nformals*WORD_SIZE);
	return access;
#else
	/* [todo] */
	assert(0);
#endif
}

F_accessList F_formals(F_frame f) {
	return f->formals;
}

/* [todo] modify to prevent var copy */
F_frame F_newFrame(Temp_label name, U_boolList formals) {
	F_frame frame = malloc(sizeof(*frame));
	frame->name = name;
	frame->nformals = 0;
	frame->formals = F_AccessList(frame, formals);
	return frame;
}

F_frag F_StringFrag(Temp_label label, string str) {
	F_frag frag = malloc(sizeof(*frag));
	frag->kind = F_stringFrag;
	frag->u.stringg.label = label;
	frag->u.stringg.str = str;
	return frag;
}

F_frag F_ProcFrag(T_stm body, F_frame frame) {
	F_frag frag = malloc(sizeof(*frag));
	frag->kind = F_procFrag;
	frag->u.proc.body = body;
	frag->u.proc.frame = frame;
	return frag;
}

F_fragList F_FragList(F_frag head, F_fragList tail) {
	F_fragList fragList = malloc(sizeof(*fragList));
	fragList->head = head;
	fragList->tail = tail;
	return fragList;
}

static Temp_temp fp = NULL;
Temp_temp F_FP(void) {
	if (!fp) fp = Temp_newtemp();
	return fp;
}

T_exp F_Exp(F_access acc, T_exp framPtr) {
#ifdef ALL_ESCAPE
	if (acc->kind == inFrame) {
		return T_Mem(T_Binop(T_plus, framPtr, T_Const(acc->u.offset)));
	}
	else {
		return T_Temp(acc->u.reg);
	}
#else 
	assert(0);
#endif
}

T_exp F_ExternalCall(string str, T_expList args) {
	return T_Call(T_Name(Temp_namedlabel(str)),args);
}
