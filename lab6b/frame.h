
/*Lab5: This header file is not complete. Please finish it with more definition.*/

#ifndef FRAME_H
#define FRAME_H

#include "temp.h"
#include "tree.h"
#include "assem.h"

// #define ALL_ESCAPE

typedef struct F_frame_ *F_frame;
typedef struct F_accessList_ *F_accessList;
typedef struct F_access_ *F_access;

extern const int F_wordSize;
Temp_map F_tempMap;

struct F_frame_ {
	Temp_label name;
	F_accessList formals;
	int nformals;
};

struct F_access_ {
	enum { inFrame, inReg } kind;
	union {
		int offset;		/* inFrame */
		Temp_temp reg; 	/* inReg */
	} u;
};

struct F_accessList_ {
	F_access head;
	F_accessList tail;
};

F_frame F_newFrame(Temp_label name, U_boolList formals);

Temp_label F_name(F_frame f);
F_accessList F_formals(F_frame f);
F_access F_allocLocal(F_frame f, bool escape);

/* declaration for fragments */
typedef struct F_frag_ *F_frag;
struct F_frag_ {
	enum {F_stringFrag, F_procFrag} kind;
	union {
		struct {Temp_label label; string str;} stringg;
		struct {T_stm body; F_frame frame;} proc;
	} u;
};

F_frag F_StringFrag(Temp_label label, string str);
F_frag F_ProcFrag(T_stm body, F_frame frame);

typedef struct F_fragList_ *F_fragList;
struct F_fragList_ {F_frag head; F_fragList tail;};
F_fragList F_FragList(F_frag head, F_fragList tail);

Temp_temp F_FP(void);
T_exp F_Exp(F_access acc, T_exp framPtr);
T_exp F_ExternalCall(string str, T_expList args);
F_access F_staticLink();

//proc entry exits
T_stm F_procEntryExit1(T_stm stm);
AS_instrList F_procEntryExit2(AS_instrList body);
AS_proc F_procEntryExit3(F_frame frame, AS_instrList body);

#endif
