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
	int offset =2*F_wordSize;

	if (curr==NULL) return NULL;

	head = malloc(sizeof(*head));
	// head->head = F_allocLocal(frame, curr->head);
	head->head = InFrame(offset);
	head->tail = NULL;
	ptr = head;
	curr = curr->tail;

	while (curr!=NULL) {
		ptr->tail = malloc(sizeof(*ptr));
		ptr = ptr->tail;
		// ptr->head = F_allocLocal(frame, curr->head);
		offset += F_wordSize;
		ptr->head = InFrame(offset);
		ptr->tail = NULL;
		curr = curr->tail;
	}
	return head;
}

F_access F_allocLocal(F_frame f, bool escape) {
#if 0
	f->nformals++;
	F_access access = InFrame(-1*f->nformals*WORD_SIZE);
	return access;
#else
	if (escape) {
		f->nformals++;
		F_access access = InFrame(-1*f->nformals*WORD_SIZE);
		return access;
	}
	else {
		F_access access = InReg(Temp_newtemp());
		return access;
	}
#endif
}

F_accessList F_formals(F_frame f) {
	return f->formals;
}

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

extern Temp_temp get_ebp();
static Temp_temp fp = NULL;
Temp_temp F_FP(void) {
	if (!fp) fp = get_ebp();
	return fp;
}

T_exp F_Exp(F_access acc, T_exp framPtr) {
	if (acc->kind == inFrame) {
		return T_Mem(T_Binop(T_plus, framPtr, T_Const(acc->u.offset)));
	}
	else {
		return T_Temp(acc->u.reg);
	}
}

T_exp F_ExternalCall(string str, T_expList args) {
	return T_Call(T_Name(Temp_namedlabel(str)),args);
}

static F_access static_acc = NULL;
F_access F_staticLink() {
	if (static_acc == NULL) {
		static_acc = InFrame(8);
	}
	return static_acc;
}

static Temp_temp temp_eax = NULL, temp_ebx = NULL, temp_ecx = NULL,
		temp_edx = NULL, temp_esi = NULL, temp_edi = NULL,
		temp_esp = NULL, temp_ebp = NULL;

static Temp_tempList temp_callersaves = NULL, temp_calleesaves = NULL, temp_all = NULL;

Temp_temp get_eax() {
	if (temp_eax) return temp_eax;
	temp_eax = Temp_newtemp();
	return temp_eax;
}

Temp_temp get_ebx() {
	if (temp_ebx) return temp_ebx;
	temp_ebx = Temp_newtemp();
	return temp_ebx;
}

Temp_temp get_ecx() {
	if (temp_ecx) return temp_ecx;
	temp_ecx = Temp_newtemp();
	return temp_ecx;
}

Temp_temp get_edx() {
	if (temp_edx) return temp_edx;
	temp_edx = Temp_newtemp();
	return temp_edx;
}

Temp_temp get_esi() {
	if (temp_esi) return temp_esi;
	temp_esi = Temp_newtemp();
	return temp_esi;
}

Temp_temp get_edi() {
	if (temp_edi) return temp_edi;
	temp_edi = Temp_newtemp();
	return temp_edi;
}

Temp_temp get_esp() {
	if (temp_esp) return temp_esp;
	temp_esp = Temp_newtemp();
	return temp_esp;
}

Temp_temp get_ebp() {
	if (temp_ebp) return temp_ebp;
	temp_ebp = Temp_newtemp();
	return temp_ebp;
}

Temp_tempList get_callersaves() {
	if (temp_callersaves) return temp_callersaves;
	temp_callersaves = Temp_TempList(get_eax(), temp_callersaves);
	temp_callersaves = Temp_TempList(get_edx(), temp_callersaves);
	temp_callersaves = Temp_TempList(get_ecx(), temp_callersaves);
	return temp_callersaves;
}

Temp_tempList get_calleesaves() {
	if (temp_calleesaves) return temp_calleesaves;
	temp_calleesaves = Temp_TempList(get_esi(), temp_callersaves);
	temp_calleesaves = Temp_TempList(get_edi(), temp_callersaves);
	temp_calleesaves = Temp_TempList(get_ebx(), temp_callersaves);
	return temp_calleesaves;
}

Temp_tempList get_color_regs() {
	if (temp_all) return temp_all;
	temp_all = Temp_TempList(get_eax(), temp_all);
	temp_all = Temp_TempList(get_ebx(), temp_all);
	temp_all = Temp_TempList(get_ecx(), temp_all);
	temp_all = Temp_TempList(get_edx(), temp_all);
	temp_all = Temp_TempList(get_edi(), temp_all);
	temp_all = Temp_TempList(get_esi(), temp_all);
	
	// temp_all = Temp_TempList(get_esp(), temp_all);
	// temp_all = Temp_TempList(get_ebp(), temp_all);
	return temp_all;
}

Temp_tempList get_all_regs() {
	if (temp_all) return temp_all;
	temp_all = Temp_TempList(get_eax(), temp_all);
	temp_all = Temp_TempList(get_ebx(), temp_all);
	temp_all = Temp_TempList(get_ecx(), temp_all);
	temp_all = Temp_TempList(get_edx(), temp_all);
	temp_all = Temp_TempList(get_edi(), temp_all);
	temp_all = Temp_TempList(get_esi(), temp_all);
	temp_all = Temp_TempList(get_esp(), temp_all);
	temp_all = Temp_TempList(get_ebp(), temp_all);
	return temp_all;
}

static Temp_map tempMap = NULL;

Temp_map F_getTempMap() {
	// if (tempMap) return tempMap;
	tempMap = Temp_empty();
	Temp_enter(tempMap, get_eax(), "%eax"); 
	Temp_enter(tempMap, get_ebx(), "%ebx"); 
	Temp_enter(tempMap, get_ecx(), "%ecx"); 
	Temp_enter(tempMap, get_edx(), "%edx"); 
	Temp_enter(tempMap, get_edi(), "%edi"); 
	Temp_enter(tempMap, get_esi(), "%esi"); 
	Temp_enter(tempMap, get_esp(), "%esp"); 
	Temp_enter(tempMap, get_ebp(), "%ebp"); 
	return tempMap;
}

T_stm F_procEntryExit1(T_stm stm) {
	Temp_temp t1 = Temp_newtemp(), t2 = Temp_newtemp(), t3 = Temp_newtemp();
	if (stm->kind == T_EXP) {
			return T_Seq(T_Move(T_Temp(t1), T_Temp(get_ebx())), 
					T_Seq(T_Move(T_Temp(t2), T_Temp(get_esi())),
					T_Seq(T_Move(T_Temp(t3), T_Temp(get_edi())),
					T_Seq(T_Move(T_Temp(get_eax()), stm->u.EXP),
					T_Seq(T_Move(T_Temp(get_ebx()), T_Temp(t1)),
					T_Seq(T_Move(T_Temp(get_esi()), T_Temp(t2)),
					T_Seq(T_Move(T_Temp(get_edi()), T_Temp(t3)),
					T_Seq(T_Move(T_Temp(get_ebx()), T_Temp(get_ebx())), 
					T_Seq(T_Move(T_Temp(get_esi()), T_Temp(get_esi())), 
					T_Seq(T_Move(T_Temp(get_edi()), T_Temp(get_edi())), 
					T_Move(T_Temp(get_eax()), T_Temp(get_eax())))))))))))); 
	} 
	return T_Seq(T_Move(T_Temp(t1), T_Temp(get_ebx())), 
			T_Seq(T_Move(T_Temp(t2), T_Temp(get_esi())),
			T_Seq(T_Move(T_Temp(t3), T_Temp(get_edi())),
			T_Seq(stm,
			T_Seq(T_Move(T_Temp(get_ebx()), T_Temp(t1)),
			T_Seq(T_Move(T_Temp(get_esi()), T_Temp(t2)),
			T_Move(T_Temp(get_edi()), T_Temp(t3)))))))); 
}

static Temp_tempList returnSink = NULL;
AS_instrList F_procEntryExit2(AS_instrList body) {
	if (!returnSink) returnSink = Temp_TempList(get_eax(), get_calleesaves());
	return AS_splice(body, AS_InstrList(AS_Oper("", NULL, returnSink, NULL), NULL));
}

AS_proc F_procEntryExit3(F_frame frame, AS_instrList body) {
	assert(0);
}