#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "tree.h"
#include "absyn.h"
#include "assem.h"
#include "frame.h"
#include "graph.h"
#include "color.h"
#include "liveness.h"
#include "flowgraph.h"
#include "regalloc.h"
#include "table.h"

static int get_local(TAB_table table, F_frame f, Temp_temp temp) {
	F_access f_access = TAB_look(table, temp);
	if (f_access == NULL) {
		f_access = F_allocLocal(f, TRUE);
		TAB_enter(table, temp, f_access);
	}
	return f_access->u.offset;
}

static Temp_temp get_newtemp(TAB_table table, Temp_temp old) {
#if 1
	Temp_temp newtemp = TAB_look(table, old);
	if (newtemp == NULL) {
		newtemp = Temp_newtemp();
		TAB_enter(table, old, newtemp);
	}
	return newtemp;
#else 
	return Temp_newtemp();
#endif
}

static void rewrite(F_frame f, Temp_tempList spill_list, AS_instrList il) {
	AS_instrList prev = NULL, curr = il;
	TAB_table temp2local = TAB_empty();

	// Temp_tempList tpp = spill_list;
	// for (; tpp; tpp=tpp->tail) {
	// 	F_allocLocal(f, TRUE);
	// }

 	Temp_map dbg_map = Temp_layerMap(F_getTempMap(), Temp_name());
	AS_printInstrList(stdout, il, dbg_map);

	while(curr) {
		AS_instr instr = curr->head;
		Temp_tempList defs = NULL, uses = NULL;
		switch (instr->kind) {
			case I_OPER:
				defs = instr->u.OPER.dst;
				uses = instr->u.OPER.src;
				break;
			case I_MOVE:
				defs = instr->u.MOVE.dst;
				uses = instr->u.MOVE.src;
				break;
			default:
				break;
		}
		if (defs || uses) {
			TAB_table old2new = TAB_empty();
			while (uses != NULL) {
				if (Temp_listHas(spill_list, uses->head)) {
					assert(prev);
					Temp_temp newtemp = get_newtemp(old2new, uses->head);
					int offset = get_local(temp2local, f, uses->head);
					AS_instr new_instr = AS_Move(
											String_fmt("    movl %d(`s0), `d0    #swap t%d in with t%d\n", offset, uses->head->num, newtemp->num),
											Temp_TempList(newtemp, NULL),
											Temp_TempList(get_ebp(), NULL));

					uses->head = newtemp;
					prev->tail = AS_InstrList(new_instr, curr);
					prev = prev->tail;
				}
				uses = uses->tail;
			}
			while (defs != NULL) {
				if (Temp_listHas(spill_list, defs->head)) {
					assert(prev);
					Temp_temp newtemp = get_newtemp(old2new, defs->head);
					int offset = get_local(temp2local, f, defs->head);
					AS_instr new_instr = AS_Move(
											String_fmt("    movl `s0, %d(`s1)    #swap t%d out with t%d\n", offset, defs->head->num, newtemp->num),
											NULL,
											Temp_TempList(newtemp, Temp_TempList(get_ebp(), NULL)));
					
					defs->head = newtemp;
					curr->tail = AS_InstrList(new_instr, curr->tail); // may cause bug?
				}
				defs = defs->tail;
			}
		}
		prev = curr;
		curr = curr->tail;
	}

}

struct RA_result RA_regAlloc(F_frame f, AS_instrList il) {
	struct RA_result ret;
	// ret.coloring=NULL;
	// ret.il=il;
	// return ret;

	G_graph g = FG_AssemFlowGraph(il, f);
	struct Live_graph live_graph = Live_liveness(g);
	Temp_map initial = Temp_layerMap(Temp_empty(), F_getTempMap());
	Temp_tempList reglist = get_color_regs();
	struct COL_result col_result = COL_color(live_graph, initial, reglist);

	if (col_result.spills != NULL) {
		rewrite(f, col_result.spills, il);
		return RA_regAlloc(f, il);
	}

	ret.coloring = col_result.coloring;
	ret.il = il;
	return ret;
}
