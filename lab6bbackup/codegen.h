#ifndef CODEGEN_H
#define CODEGEN_H

#include "assem.h"
#include "frame.h"

AS_instrList F_codegen(F_frame f, T_stmList stmList);
AS_instrList epilogue(F_frame f, AS_instrList instr_list);
AS_instrList prologue(F_frame f, AS_instrList instr_list);

extern Temp_temp get_eax();
extern Temp_temp get_ebx();
extern Temp_temp get_ecx();
extern Temp_temp get_edx();
extern Temp_temp get_esi();
extern Temp_temp get_edi();
extern Temp_temp get_esp();
extern Temp_temp get_ebp();
extern Temp_tempList get_all_regs();
extern Temp_tempList get_color_regs();
extern Temp_tempList get_calleesaves();
extern Temp_tempList get_callersaves();
extern Temp_map F_getTempMap();

#endif
