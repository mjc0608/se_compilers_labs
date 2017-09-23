/* function prototype from regalloc.c */
struct RA_result {Temp_map coloring; AS_instrList il;};
struct RA_result RA_regAlloc(F_frame f, AS_instrList il);

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