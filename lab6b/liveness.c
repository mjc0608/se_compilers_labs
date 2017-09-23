#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "tree.h"
#include "absyn.h"
#include "assem.h"
#include "frame.h"
#include "graph.h"
#include "flowgraph.h"
#include "liveness.h"
#include "table.h"
#include "regalloc.h"

static void enterLiveMap(G_table t, G_node flowNode, Temp_tempList temps) {
	G_enter(t, flowNode, temps);
}

static Temp_tempList lookupLiveMap(G_table t, G_node flownode) {
	return (Temp_tempList)G_look(t, flownode);
}

static bool inSet(Temp_tempList tl, Temp_temp tt) {
	for (; tl; tl=tl->tail) {
		if (tl->head == tt) return TRUE;
	}
	return FALSE;
}

static Temp_tempList unionSet(Temp_tempList t1, Temp_tempList t2) {
	Temp_tempList p1 = t1, p2 = t2;
	Temp_tempList r = NULL;
	for (; p1; p1=p1->tail) {
		Temp_temp temp1 = p1->head;
		if (!inSet(r, temp1)) r = Temp_TempList(temp1, r);
	}
	for (; p2; p2=p2->tail) {
		Temp_temp temp2 = p2->head;
		if (!inSet(r, temp2)) r = Temp_TempList(temp2, r);
	}
	return r;
}

static Temp_tempList differenceSet(Temp_tempList t1, Temp_tempList t2) {
	Temp_tempList p1 = t1, p2 = t2;
	Temp_tempList r = NULL;
	for (; p1; p1=p1->tail) {
		Temp_temp temp1 = p1->head;
		if (!inSet(t2, temp1)) {
			r = Temp_TempList(temp1, r);
		}
	}
	return r;
}

static G_node getNode(TAB_table table, G_graph graph, Temp_temp temp) {
	G_node node = TAB_look(table, temp);
	if (node == NULL) {
		node = G_Node(graph, temp);
		TAB_enter(table, temp, node);
		// printf("init node t%d\n", temp->num);
	}
	// printf("get node t%d\n", temp->num);

	return node;
}

static int sizeOfSet(Temp_tempList l) {
	int size = 0;
	for (; l; l=l->tail) {
		size++;
	}
	return size;
}

static bool isPrecolored(Temp_temp t) {
	if (t==get_eax()) return TRUE;
	if (t==get_ebx()) return TRUE;
	if (t==get_ecx()) return TRUE;
	if (t==get_edx()) return TRUE;
	if (t==get_esi()) return TRUE;
	if (t==get_edi()) return TRUE;
	if (t==get_esp()) return TRUE;
	if (t==get_ebp()) return TRUE;
	return FALSE;
}

Live_moveList Live_MoveList(G_node src, G_node dst, Live_moveList tail) {
	Live_moveList lm = (Live_moveList) checked_malloc(sizeof(*lm));
	lm->src = src;
	lm->dst = dst;
	lm->tail = tail;
	return lm;
}

Temp_temp Live_gtemp(G_node n) {
	return G_nodeInfo(n);
}

struct Live_graph Live_liveness(G_graph flow) {
	struct Live_graph lg;
	G_graph g = G_Graph();
	Live_moveList moves = NULL;
	G_table gt_in = G_empty();
	G_table gt_out = G_empty();
	TAB_table temp2node = TAB_empty();

	G_nodeList nodes = reverseNodeList(G_nodes(flow));
	G_nodeList p = nodes;

	bool modified = TRUE;
	while (modified == TRUE) {
		modified = FALSE;
		p = nodes;
		while (p!=NULL) {
			G_node n = p->head;
			Temp_tempList oldin = lookupLiveMap(gt_in, n);
			Temp_tempList oldout = lookupLiveMap(gt_out, n);
			Temp_tempList newin = unionSet(FG_use(n), differenceSet(oldout, FG_def(n)));
			Temp_tempList newout = NULL;

			G_nodeList succs = G_succ(n);
			for (; succs!=NULL; succs=succs->tail) {
				G_node succ = succs->head;
				newout = unionSet(newout, lookupLiveMap(gt_in, succ));
			}
			if (sizeOfSet(newin)!=sizeOfSet(oldin)) {
				modified = TRUE;
				enterLiveMap(gt_in, n, newin);
			}
			if (sizeOfSet(newout)!=sizeOfSet(oldout)) {
				modified = TRUE;
				enterLiveMap(gt_out, n, newout);
			}
			p = p->tail;
		}
	}

	p = G_nodes(flow);
	while (p!=NULL) {
		G_node n = p->head;
		Temp_tempList def = FG_def(n);
		Temp_tempList use = FG_use(n);
		Temp_tempList tl = lookupLiveMap(gt_out, n);
		
		// for coalesce
		if (FG_isMove(n)) {
			if (def==NULL || def->head==NULL) goto skip_ismove;
			if (use==NULL || use->head==NULL) goto skip_ismove;
			if (def->head==get_ebp() || def->head==get_esp()) goto skip_ismove;
			if (use->head==get_ebp() || use->head==get_esp()) goto skip_ismove;
			if (isPrecolored(use->head) && isPrecolored(def->head)) goto skip_ismove;
			tl = differenceSet(tl, use);
			moves = Live_MoveList(getNode(temp2node, g, use->head), getNode(temp2node, g, def->head), moves);
			// printf("move t%d t%d\n", use->head->num, def->head->num);
		}

skip_ismove:

		while (def!=NULL) {
			Temp_temp def_temp = def->head;
			G_node def_node = getNode(temp2node, g, def_temp);

			Temp_tempList tl_p = tl;
			for (; tl_p!=NULL; tl_p=tl_p->tail) {
				Temp_temp tt = tl_p->head;
				G_node g_node = getNode(temp2node, g, tt);
				// printf("link t%d <-> t%d\n", tt->num, def_temp->num);
				if (def_node == g_node) continue;
				G_addEdge(def_node, g_node);
				G_addEdge(g_node, def_node);
			}
			def = def->tail;
		}

		//some node are never fucked.
		while (use!=NULL) {
			Temp_temp use_temp = use->head;
			G_node use_node = getNode(temp2node, g, use_temp);
			use = use->tail;
		}

		p = p->tail;
	}

	// printf("liveness:\n");
	// G_show(stdout, G_nodes(g), Temp_dumpTemp);
	// printf("\n");

	lg.graph = g;
	lg.moves = moves;

	return lg;
}


