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
#include "color.h"
#include "table.h"
#include "liveness.h"

static G_nodeList precolored = NULL;
static G_nodeList simplifyWorklist = NULL;
static G_nodeList freezeWorklist = NULL;
static G_nodeList spillWorklist = NULL;
static G_nodeList spilledNodes = NULL;
static G_nodeList coalescedNodes = NULL;
static G_nodeList coloredNodes = NULL;
static G_nodeList selectStack = NULL;

static COL_moveList coalescedMoves = NULL;
static COL_moveList constrainedMoves = NULL;
static COL_moveList frozenMoves = NULL;
static COL_moveList worklistMoves = NULL;
static COL_moveList activeMoves = NULL;

static int nnodes;
static int K;

static bool *adjSet = NULL;
static G_nodeList *adjList = NULL;
static int *degree = NULL;

static COL_moveList *moveList = NULL;
static G_node *alias = NULL;

static Temp_map color = NULL;
static Temp_tempList reglist = NULL;

Temp_tempList spillregs = NULL;

COL_stringList COL_StringList(string head, COL_stringList tail) {
	COL_stringList r = checked_malloc(sizeof(*r));
	r->head = head;
	r->tail = tail;
	return r;
}

COL_moveItem COL_MoveItem(G_node dst, G_node src) {
	COL_moveItem item = checked_malloc(sizeof(*item));
	item->dst = dst;
	item->src = src;
	return item;
}

COL_moveList COL_MoveList(COL_moveItem item, COL_moveList list) {
	COL_moveList r = checked_malloc(sizeof(*r));
	r->head = item;
	r->tail = list;
	return r;
}

void init_adjSet(int n) {
	int i;
	adjSet = checked_malloc(n*n*sizeof(bool));
	memset(adjSet, 0, n*n*sizeof(bool));
}

void init_adjList(int n) {
	adjList = checked_malloc(n*sizeof(G_nodeList));
	int i;
	for (i=0; i<n; i++) {
		adjList[i] = NULL;
	}
}

void init_degree(int n) {
	degree = checked_malloc(n*sizeof(int));
	int i;
	memset(degree, 0, n*sizeof(int));
}

void init_moveList(int n) {
	moveList = checked_malloc(n*sizeof(COL_moveList));
	int i;
	for (i=0; i<n; i++) {
		moveList[i] = NULL;
	}
}

void init_alias(int n) {
	alias = checked_malloc(n*sizeof(G_node));
	int i;
	for (i=0; i<n; i++) {
		alias[i] = NULL;
	}
}

void init_coloring(int n, Temp_map initial_map, Temp_tempList regs) {
	precolored = NULL;
	simplifyWorklist = NULL;
	freezeWorklist = NULL;
	spillWorklist = NULL;
	spilledNodes = NULL;
	coalescedNodes = NULL;
	coloredNodes = NULL;
	selectStack = NULL;

	coalescedMoves = NULL;
	constrainedMoves = NULL;
	frozenMoves = NULL;
	worklistMoves = NULL;
	activeMoves = NULL;

	nnodes = n;
	K = Temp_sizeOfTempList(regs);
	init_adjSet(n);
	init_adjList(n);
	init_degree(n);
	init_moveList(n);
	init_alias(n);

	color = initial_map;
	reglist = regs;
}

static bool G_nodeListIsEmpty(G_nodeList list) {
	return list == NULL;
}

static bool COL_moveListIsEmpty(COL_moveList list) {
	return list == NULL;
}

static bool G_nodeListHas(G_nodeList list, G_node n) {
	G_nodeList l = list;
	for(; l; l=l->tail) {
		if (l->head == n) return TRUE;
	}
	return FALSE;
}

static bool COL_moveListHas(COL_moveList list, COL_moveItem item) {
	for (; list; list=list->tail) {
		if (list->head == item) return TRUE;
	}
	return FALSE;
}

static void G_pushStack(G_nodeList *stack, G_node node) {
	assert(stack);
	*stack = G_NodeList(node, *stack);
}

static G_node G_lookStack(G_nodeList *stack) {
	assert(stack);
	return (*stack)->head;
}

static G_node G_popStack(G_nodeList *stack) {
	assert(stack);
	G_node node = (*stack)->head;
	*stack = (*stack)->tail;
	return node;
}

static G_node G_lookList(G_nodeList *list) {
	assert(list);
	return (*list)->head;
}

static COL_moveItem COL_lookMoveList(COL_moveList *list) {
	assert(list);
	return (*list)->head;
}

static G_node G_peekList(G_nodeList *list) {
	assert(list);
	G_node node = (*list)->head;
	*list = (*list)->tail;
	return node;
}

static COL_moveItem COL_peekMoveList(COL_moveList *list) {
	assert(list);
	COL_moveItem item = (*list)->head;
	*list = (*list)->tail;
	return item;
}

static void G_appendList(G_nodeList *list, G_node node) {
	assert(list);
	// if (G_nodeListHas(*list, node)) assert(0);
	if (G_nodeListHas(*list, node)) return;
	G_nodeList l = G_NodeList(node, *list);
	*list = l;
}

static void COL_appendMoveList(COL_moveList *list, COL_moveItem item) {
	assert(list);
	// if (COL_moveListHas(*list, item)) assert(0);
	if (COL_moveListHas(*list, item)) return;
	COL_moveList l = COL_MoveList(item, *list);
	*list = l;
}

static bool G_deleteList(G_nodeList *list, G_node node) {
	assert(list);
	G_nodeList prev = NULL, curr = *list;
	for (; curr; curr=curr->tail) {
		if (curr->head == node) {
			if (prev) {
				prev->tail = curr->tail;
				return TRUE;
			} 
			else {
				*list = curr->tail;
				return TRUE;
			}
		}
		prev = curr;
	}
	return FALSE;
}

static bool COL_deleteMoveList(COL_moveList *list, COL_moveItem item) {
	assert(list);
	COL_moveList prev = NULL, curr = *list;
	for (; curr; curr=curr->tail) {
		if (curr->head == item) {
			if (prev) {
				prev->tail = curr->tail;
				return TRUE;
			}
			else {
				*list = curr->tail;
				return TRUE;
			}
		}
		prev = curr;
	}
	return FALSE;
}

static bool String_deleteList(COL_stringList *list, string s) {
	assert(list);
	COL_stringList prev = NULL, curr = *list;
	for (; curr; curr=curr->tail) {
		if (curr->head == s) {
			if (prev) {
				prev->tail = curr->tail;
				return TRUE;
			}
			else {
				*list = curr->tail;
				return TRUE;
			}
		}
		prev = curr;

	}
	return FALSE;
}

static bool isLink(int m, int n) {
	return adjSet[m*nnodes+n];
}

static bool G_isPrecolored(G_node node) {
	return G_nodeListHas(precolored, node);
}

static bool Temp_isPrecolored(Temp_temp temp) {
	G_nodeList l = precolored;
	for(; l; l=l->tail) {
		if (l->head->info == temp) return TRUE;
	}
	return FALSE;
}

static void addEdge(G_node node1, G_node node2) {
	int m = node1->mykey;
	int n = node2->mykey;
	if (m!=n && !isLink(m, n)) {
		adjSet[m*nnodes+n] = TRUE;
		adjSet[n*nnodes+m] = TRUE;
		if (!G_isPrecolored(node1)) {
			if (!G_nodeListHas(adjList[m], node2)) {
				G_appendList(&adjList[m], node2);
				degree[m]++;
			}
		}
		if (!G_isPrecolored(node2)) {
			if (!G_nodeListHas(adjList[n], node1)) {
				G_appendList(&adjList[n], node1);
				degree[n]++;
			}
		}
	} 
}

static G_nodeList G_unionSet(G_nodeList s1, G_nodeList s2) {
	G_nodeList p1 = s1, p2 = s2;
	G_nodeList ret = NULL;
	for (; p1; p1=p1->tail) {
		if (!G_nodeListHas(ret, p1->head)) {
			G_appendList(&ret, p1->head);
		}
	}
	for (; p2; p2=p2->tail) {
		if (!G_nodeListHas(ret, p2->head)) {
			G_appendList(&ret, p2->head);
		}
	}
	return ret;
}

static G_nodeList G_differSet(G_nodeList s1, G_nodeList s2) {
	G_nodeList p1 = s1, p2 = s2;
	G_nodeList ret = NULL;
	for (; p1; p1=p1->tail) {
		G_node n = p1->head;
		if(!G_nodeListHas(s2, n)) {
			G_appendList(&ret, n);
		}
	}
	return ret;
}

static COL_moveList COL_interMoveList(COL_moveList l1, COL_moveList l2) {
	COL_moveList r = NULL;
	for (; l1; l1=l1->tail) {
		if (COL_moveListHas(l2, l1->head)) {
			r = COL_MoveList(l1->head, r);
		}
	}
	return r;
}

static COL_moveList COL_unionMoveList(COL_moveList l1, COL_moveList l2) {
	COL_moveList r = NULL;
	for (; l1; l1=l1->tail) {
		if (!COL_moveListHas(r, l1->head)) {
			r = COL_MoveList(l1->head, r);
		}
	}
	for (; l2; l2=l2->tail) {
		if (!COL_moveListHas(r, l2->head)) {
			r = COL_MoveList(l2->head, r);
		}
	}
	return r;
}

static G_nodeList adjacent(G_node node) {
	return G_differSet(
		adjList[node->mykey],
		G_unionSet(
			selectStack,
			coalescedNodes
		)
	);
}

static bool moveReleated(G_node n) {
	if (moveList[n->mykey]==NULL) return FALSE;
	else return TRUE;
}

static COL_moveList nodeMoves(G_node n) {
	return COL_interMoveList(
		moveList[n->mykey],
		COL_unionMoveList(
			activeMoves,
			worklistMoves
		)
	);
}

static G_node getAlias(G_node n) {
	if (G_nodeListHas(coalescedNodes, n)) {
		return getAlias(alias[n->mykey]);
	}
	else {
		return n;
	}
}

static void addWorkList(G_node u) {
	if (!G_nodeListHas(precolored, u) && !moveReleated(u) && degree[u->mykey]<K) {
		G_deleteList(&freezeWorklist, u);
		G_appendList(&simplifyWorklist, u);
	}
}

static bool OK(G_node t, G_node r) {
	return degree[t->mykey]<K || G_isPrecolored(t) || isLink(t->mykey, r->mykey);
}

static bool allOK(G_node u, G_node v) {
	G_nodeList adjv = adjacent(v);
	for (; adjv; adjv=adjv->tail) {
		G_node t = adjv->head;
		if (!OK(t, u)) return FALSE;
	}
	return TRUE;
}

static bool conservative(G_nodeList list) {
	int k = 0;
	for (; list; list=list->tail) {
		G_node n = list->head;
		if (degree[n->mykey]>=K) k++;
	}
	return (k<K);
}

static void decrementDegree(G_node);
static void enableMoves(G_nodeList);

static void combine(G_node u, G_node v) {
	if (G_nodeListHas(freezeWorklist, v)) {
		G_deleteList(&freezeWorklist, v);
	}
	else {
		G_deleteList(&spillWorklist, v);
	}
	G_appendList(&coalescedNodes, v);
	alias[v->mykey] = u;
	moveList[u->mykey] = COL_unionMoveList(moveList[u->mykey], moveList[v->mykey]);
	enableMoves(G_NodeList(v, NULL));

	// printf("combine: %d %d\n", u->mykey, v->mykey);
	
	G_nodeList adjv = adjacent(v);
	for (; adjv; adjv=adjv->tail) {
		addEdge(adjv->head, u);
		decrementDegree(adjv->head);
	}
	if (degree[u->mykey]>=K && G_nodeListHas(freezeWorklist, u)) {
		G_deleteList(&freezeWorklist, u);
		G_appendList(&spillWorklist, u);
	}
}

static void freezeMoves(G_node u) {
	G_node v = NULL;
	COL_moveList moves = nodeMoves(u);
	for (; moves; moves=moves->tail) {
		COL_moveItem m = moves->head;
		if (getAlias(m->dst)==getAlias(u)) {
			v = getAlias(m->src);
		}
		else {
			v = getAlias(m->dst);
		}
		COL_deleteMoveList(&activeMoves, m);
		COL_appendMoveList(&frozenMoves, m);
		if ((nodeMoves(v)==NULL && degree[v->mykey]<K) && !G_isPrecolored(v)) {
			G_deleteList(&freezeWorklist, v);
			G_appendList(&simplifyWorklist, v);
		} 
	}
}

static void freeze() {
	assert(freezeWorklist);
	G_node u = G_peekList(&freezeWorklist);
	G_appendList(&simplifyWorklist, u);
	freezeMoves(u);
}


static void makeWorklist(G_nodeList nodes_initial) {
	G_nodeList p = nodes_initial;
	for (; p; p=p->tail) {
		G_node node = p->head;
		if (degree[node->mykey]>=K) {
			G_appendList(&spillWorklist, node);
		}
		else if (moveReleated(node)) {
			G_appendList(&freezeWorklist, node);
		}
		else {
			G_appendList(&simplifyWorklist, node);
		}
	}
}

static void makeWorklistMoves(Live_moveList moves) {
	for (; moves; moves=moves->tail) {
		COL_moveItem item = COL_MoveItem(moves->dst, moves->src);
		COL_appendMoveList(&worklistMoves, item);
		
		COL_appendMoveList(&moveList[moves->dst->mykey], item);
		COL_appendMoveList(&moveList[moves->src->mykey], item);
	}
}

static void decrementDegree(G_node node) {
	int d = degree[node->mykey];
	degree[node->mykey]--;
	if (d == K) {
		enableMoves(G_NodeList(node, adjacent(node)));
		if (G_nodeListHas(selectStack, node)) return;
		G_deleteList(&spillWorklist, node);
		if (!moveReleated(node)) {
			G_appendList(&simplifyWorklist, node);
		}
		else {
			G_appendList(&freezeWorklist, node);
		}
	}
}

static void enableMoves(G_nodeList nodes) {
	for (; nodes; nodes=nodes->tail) {
		COL_moveList moves = nodeMoves(nodes->head);
		for (; moves; moves=moves->tail) {
			COL_moveItem move = moves->head;
			if (COL_moveListHas(activeMoves, move)) {
				COL_deleteMoveList(&activeMoves, move);
				COL_appendMoveList(&worklistMoves, move);
			}
		}
	}
}

static void simplify() {
	if (simplifyWorklist!=NULL) {
		G_node node = G_peekList(&simplifyWorklist);
		G_pushStack(&selectStack, node);
		G_nodeList l = adjacent(node);
		for (; l; l=l->tail) {
			decrementDegree(l->head);
		}
	}
}

static void coalesce() {
	COL_moveItem m = COL_peekMoveList(&worklistMoves);
	G_node x = getAlias(m->src);
	G_node y = getAlias(m->dst);
	G_node u = NULL, v = NULL;
	if (G_isPrecolored(y)) {
		u = y;
		v = x;
	}
	else {
		u = x;
		v = y;
	}
	if (u==v) {
		COL_appendMoveList(&coalescedMoves, m);
		addWorkList(u);
	}
	else if (G_isPrecolored(v) || isLink(u->mykey, v->mykey)) {
		COL_appendMoveList(&constrainedMoves, m);
		addWorkList(u);
		addWorkList(v);
	}
	else if ((G_isPrecolored(u) && allOK(u, v)) 
		|| (!G_isPrecolored(u) && conservative(G_unionSet(adjacent(u), adjacent(v))))) {
		COL_appendMoveList(&coalescedMoves, m);
		combine(u, v);
		addWorkList(u);
	}
	else {
		COL_appendMoveList(&activeMoves, m);
	}
}

static bool Temp_tempListHas(Temp_tempList l, Temp_temp t) {
	for (; l; l=l->tail) {
		if (l->head == t) return TRUE;
	}
	return FALSE;
}

static void selectSpill() {
#if 0
	G_node node = G_peekList(&spillWorklist);
	G_appendList(&simplifyWorklist, node);
#else
	G_nodeList p = spillWorklist;
	G_node node_degree_max = spillWorklist->head;
	for (; p; p=p->tail) {
		if (degree[p->head->mykey]>=degree[node_degree_max->mykey]) {
			node_degree_max = p->head;
		}
	}
	// assert(!Temp_tempListHas(spillregs, node_degree_max->info));
	G_deleteList(&spillWorklist, node_degree_max);
	G_appendList(&simplifyWorklist, node_degree_max);
	freezeMoves(node_degree_max);
#endif
}

extern Temp_map F_getTempMap();

static string get_reg_str(Temp_temp reg) {
	Temp_map temp_map = F_getTempMap();
	return Temp_look(temp_map, reg);
}

static COL_stringList allColors() {
	COL_stringList sl = NULL;
	Temp_tempList c = reglist;
	for (; c; c=c->tail) {
		sl = COL_StringList(get_reg_str(c->head), sl);
	}
	return sl;
}

static void Temp_dumpTempWithName(void* temp) {
  Temp_temp t = temp;
  printf("t%d  ", t->num);
  string c = Temp_look(color, temp);
  string fck = Temp_isPrecolored(t)?"p":" ";
  if (c!=NULL) printf("%s %s ", c, fck);
  else printf("spilled");
}

static void assignColor() {
	while (selectStack!=NULL) {
		G_node n = G_popStack(&selectStack);
		COL_stringList okColors = allColors();
		G_nodeList g_nodelist = adjList[n->mykey];
		for (; g_nodelist; g_nodelist=g_nodelist->tail) {
			G_nodeList tmplist = G_unionSet(coloredNodes, precolored);
			if (okColors==NULL) break;
			if (G_nodeListHas(tmplist, getAlias(g_nodelist->head))) {
				string color_str = Temp_look(color, G_nodeInfo(getAlias(g_nodelist->head)));
				String_deleteList(&okColors, color_str);
			}
		}
		if (okColors==NULL) {
			G_appendList(&spilledNodes, n);
			// printf("spill t%d out\n", ((Temp_temp)n->info)->num);
		}
		else {
			G_appendList(&coloredNodes, n);
			Temp_enter(color, G_nodeInfo(n), okColors->head);
			Temp_temp t = G_nodeInfo(n);
			// printf("color t%d %s\n", t->num, okColors->head);
		}
	}
	G_nodeList nodes = coalescedNodes;
	for (; nodes; nodes=nodes->tail) {
		string color_alias = Temp_look(color, G_nodeInfo(getAlias(nodes->head)));
		Temp_enter(color, G_nodeInfo(nodes->head), color_alias);
	}
}


struct COL_result COL_color(struct Live_graph ig, Temp_map initial, Temp_tempList regs) {
	G_graph graph = ig.graph;
	Live_moveList moves = ig.moves;
	G_nodeList gp = G_nodes(graph), g_nodes = NULL;
	int i;

	init_coloring(graph->nodecount, initial, regs);

	for (; gp; gp=gp->tail) {
		G_node node = gp->head;
		if (Temp_look(initial, G_nodeInfo(node))!=NULL) {
			G_appendList(&precolored, node);
			Temp_temp t = G_nodeInfo(node);
			string fuck = Temp_look(initial, G_nodeInfo(node));
			// printf("t%d -> %s\n", t->num, fuck);
		}
		else {
			G_appendList(&g_nodes, node);
			Temp_temp t = G_nodeInfo(node);
			// printf("t%d is scanned\n", t->num);
		}

	}

	gp = G_nodes(graph);
	for (; gp; gp=gp->tail) {
		G_node node = gp->head;

		//init adjSet and adjList
		G_nodeList adjs = G_adj(node);
		for (; adjs; adjs=adjs->tail) {
			addEdge(node, adjs->head);
		}
	}

	makeWorklistMoves(moves);
	makeWorklist(g_nodes);
	// for (i = 0; i<nnodes; i++) {printf("degree[%d] = %d\n", i, degree[i]);}
	
	do {
		if (!G_nodeListIsEmpty(simplifyWorklist)) {
			simplify();
		}
		else if (!COL_moveListIsEmpty(worklistMoves)) {
			coalesce();
		}
		else if (!G_nodeListIsEmpty(freezeWorklist)) {
			freeze();
		}
		else {
			selectSpill();
		}
	} while (!G_nodeListIsEmpty(simplifyWorklist) || !G_nodeListIsEmpty(spillWorklist)
		|| !G_nodeListIsEmpty(freezeWorklist) || !COL_moveListIsEmpty(worklistMoves));

	assignColor();
	// G_show(stdout, G_nodes(graph), Temp_dumpTempWithName);
	// for (i = 0; i<nnodes; i++) {printf("degree[%d] = %d\n", i, degree[i]);}

	struct COL_result col_result;
	col_result.coloring = Temp_layerMap(color, F_getTempMap());
	Temp_tempList spills = NULL;
	for (; spilledNodes; spilledNodes=spilledNodes->tail) {
		spills = Temp_TempList(G_nodeInfo(spilledNodes->head), spills);
	}
	col_result.spills = spills;

	return col_result;
}
