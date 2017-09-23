#include <stdio.h>
#include <string.h>
#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "tree.h"
#include "absyn.h"
#include "assem.h"
#include "frame.h"
#include "graph.h"
#include "flowgraph.h"
#include "errormsg.h"
#include "table.h"


Temp_tempList FG_def(G_node n) {
	AS_instr instr = NULL;
	if (n==NULL) return NULL;
	instr = G_nodeInfo(n);
	if (instr == NULL) return NULL;
	switch (instr->kind) {
		case I_MOVE: return instr->u.MOVE.dst;
		case I_OPER: return instr->u.OPER.dst;
		case I_LABEL: return NULL;
		default: assert(0);
	}
	return NULL;
}

Temp_tempList FG_use(G_node n) {
	AS_instr instr = NULL;
	if (n==NULL) return NULL;
	instr = G_nodeInfo(n);
	if (instr == NULL) return NULL;
	switch (instr->kind) {
		case I_MOVE: return instr->u.MOVE.src;
		case I_OPER: return instr->u.OPER.src;
		case I_LABEL: return NULL;
		default: assert(0);
	}
	return NULL;
}

bool FG_isMove(G_node n) {
	AS_instr instr = NULL;
	if (n == NULL) return FALSE;
	instr = G_nodeInfo(n);
	if (instr == NULL) return FALSE;

	return instr->kind == I_MOVE;
}

static bool is_dir_jump(AS_instr instr) {
	if (instr->kind!=I_OPER) return FALSE;
	string ins_str = instr->u.OPER.assem;
	if (strstr(ins_str, "jmp ")!=NULL) return TRUE;
	else return FALSE;
}

G_graph FG_AssemFlowGraph(AS_instrList il, F_frame f) {
	G_graph g = G_Graph();
	G_nodeList rest_nodes = NULL;
	TAB_table label_table = TAB_empty();
	AS_instrList p = il;
	G_node curr = NULL, prev = NULL;

	for(; p!=NULL; p=p->tail) {
		AS_instr instr = p->head;
		curr = G_Node(g, instr);
		if (prev!=NULL && !is_dir_jump(G_nodeInfo(prev))) G_addEdge(prev, curr);
		// if (prev!=NULL) G_addEdge(prev, curr);
		switch (instr->kind) {
			case I_LABEL:
				TAB_enter(label_table, instr->u.LABEL.label, curr);
				break;
			case I_OPER:
				if (instr->u.OPER.jumps==NULL) break;
				rest_nodes = G_NodeList(curr, rest_nodes);
				break;
			case I_MOVE:
				break;
			default:
				assert(0);
		}
		prev = curr;
	}

	for (; rest_nodes!=0; rest_nodes=rest_nodes->tail) {
		curr = rest_nodes->head;
		Temp_labelList labelList = ((AS_instr)G_nodeInfo(curr))->u.OPER.jumps->labels;
		G_node targ = NULL;
		for (; labelList!=NULL; labelList=labelList->tail) {
			targ = TAB_look(label_table, labelList->head);
			if (targ!=NULL) G_addEdge(curr, targ);
		}
	}

	// G_show(stdout, G_nodes(g), AS_dumpAssem);

	return g;
}
