/*
 * main.c
 */

#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "types.h"
#include "absyn.h"
#include "errormsg.h"
#include "temp.h" /* needed by translate.h */
#include "tree.h" /* needed by frame.h */
#include "assem.h"
#include "frame.h" /* needed by translate.h and printfrags prototype */
#include "translate.h"
#include "env.h"
#include "semant.h" /* function prototype for transProg */
#include "canon.h"
#include "prabsyn.h"
#include "printtree.h"
//#include "escape.h" /* needed by escape analysis */
#include "parse.h"
#include "codegen.h"
#include "regalloc.h"
#include <string.h>

extern bool anyErrors;

/* print the assembly language instructions to filename.s */
static void doProc(FILE *out, F_frame frame, T_stm body)
{
 AS_proc proc;
 T_stmList stmList;
 AS_instrList iList;

 F_tempMap = Temp_empty();
 Temp_map dbg_map = Temp_layerMap(F_getTempMap(), Temp_name());

//  printStmList(stdout, T_StmList(body, NULL));


 stmList = C_linearize(body);
 stmList = C_traceSchedule(C_basicBlocks(stmList));
//  printStmList(stdout, stmList);
 iList  = F_codegen(frame, stmList); /* 9 */

 struct RA_result ra = RA_regAlloc(frame, iList);  /* 10, 11 */

 iList = prologue(frame, iList);
 iList = epilogue(frame, iList);

 RA_postAlloc(iList, ra.coloring);

 Temp_map dbg_map2 = Temp_layerMap(ra.coloring, Temp_name());

  // AS_printInstrList (stdout, iList, dbg_map2);


//  fprintf(out, "BEGIN function\n");
 AS_printInstrList (out, iList, Temp_layerMap(F_tempMap,ra.coloring));
//  fprintf(out, "END function\n\n");
//  AS_printInstrList (stdout, iList, dbg_map);
}

int main(int argc, string *argv)
{
 A_exp absyn_root;
 S_table base_env, base_tenv;
 F_fragList frags, p;
 char outfile[100];
 FILE *out = stdout;

 if (argc==2) {
   absyn_root = parse(argv[1]);
   if (!absyn_root)
     return 1;
     
#if 0
   pr_exp(out, absyn_root, 0); /* print absyn data structure */
   fprintf(out, "\n");
#endif
	//If you have implemented escape analysis, uncomment this
   Esc_findEscape(absyn_root); /* set varDec's escape field */

   frags = SEM_transProg(absyn_root);
   p = frags;
   if (anyErrors) return 1; /* don't continue */

   /* convert the filename */
   sprintf(outfile, "%s.s", argv[1]);
   out = fopen(outfile, "w");
   /* Chapter 8, 9, 10, 11 & 12 */
   fprintf(out, ".text\n.globl tigermain\n.type tigermain, @function\n\n");
   for (; p; p=p->tail)
     if (p->head->kind == F_procFrag) 
       doProc(out, p->head->u.proc.frame, p->head->u.proc.body);

   p = frags;
   fprintf(out, ".section .rodata\n");
   for (; p; p=p->tail)
     if (p->head->kind == F_stringFrag) {
       int l = strlen(p->head->u.stringg.str);
       char *a = (char*)&l;
       fprintf(out, "%s:\n.string \"%c%c%c%c%s\"\n", S_name(p->head->u.stringg.label), a[0], a[1], a[2], a[3], String_toPut(p->head->u.stringg.str));
     }

   fclose(out);
   return 0;
 }
 EM_error(0,"usage: tiger file.tig");
 return 1;
}
