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

static G_nodeList precolored = NULL;
// static G_nodeList initial = NULL;
static G_nodeList simplifyWorklist = NULL;
// static G_nodeList freezeWorklist = NULL;
static G_nodeList spillWorklist = NULL;
static G_nodeList spilledNodes = NULL;
// static G_nodeList coalescedNodes = NULL;
static G_nodeList coloredNodes = NULL;
static G_nodeList selectStack = NULL;

static int nnodes;
static int K;

static bool *adjSet = NULL;
static G_nodeList *adjList = NULL;
static int *degree = NULL;
static Temp_map color = NULL;
static Temp_tempList reglist = NULL;

COL_stringList COL_StringList(string head, COL_stringList tail) {
	COL_stringList r = checked_malloc(sizeof(*r));
	r->head = head;
	r->tail = tail;
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

void init_coloring(int n, Temp_map initial_map, Temp_tempList regs) {
	precolored = NULL;
	// initial = NULL;
	simplifyWorklist = NULL;
	// freezeWorklist = NULL;
	spillWorklist = NULL;
	spilledNodes = NULL;
	// coalescedNodes = NULL;
	coloredNodes = NULL;
	selectStack = NULL;

	nnodes = n;
	K = Temp_sizeOfTempList(regs);
	init_adjSet(n);
	init_adjList(n);
	init_degree(n);
	
	color = initial_map;
	// initial = initial_map;
	reglist = regs;
}

static bool G_nodeListIsEmpty(G_nodeList list) {
	return list == NULL;
}

static bool G_nodeListHas(G_nodeList list, G_node n) {
	G_nodeList l = list;
	for(; l; l=l->tail) {
		if (l->head == n) return TRUE;
	}
	return FALSE;
}

static void G_pushstack(G_nodeList *stack, G_node node) {
	assert(stack);
	*stack = G_NodeList(node, *stack);
}

static G_node G_lookstack(G_nodeList *stack) {
	assert(stack);
	return (*stack)->head;
}

static G_node G_popstack(G_nodeList *stack) {
	assert(stack);
	G_node node = (*stack)->head;
	*stack = (*stack)->tail;
	return node;
}

static G_node G_looklist(G_nodeList *list) {
	assert(list);
	return (*list)->head;
}

static G_node G_peeklist(G_nodeList *list) {
	assert(list);
	G_node node = (*list)->head;
	*list = (*list)->tail;
	return node;
}

static void G_appendlist(G_nodeList *list, G_node node) {
	assert(list);
	G_nodeList l = G_NodeList(node, *list);
	*list = l;
}

static bool G_deletelist(G_nodeList *list, G_node node) {
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

static bool String_deletelist(COL_stringList *list, string s) {
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

static bool is_link(int m, int n) {
	return adjSet[m*nnodes+n];
}

static bool is_precolored(G_node node) {
	return G_nodeListHas(precolored, node);
}

static bool temp_precolored(Temp_temp temp) {
	G_nodeList l = precolored;
	for(; l; l=l->tail) {
		if (l->head->info == temp) return TRUE;
	}
	return FALSE;
}

static void add_edge(G_node node1, G_node node2) {
	int m = node1->mykey;
	int n = node2->mykey;
	if (m!=n && !is_link(m, n)) {
		adjSet[m*nnodes+n] = TRUE;
		adjSet[n*nnodes+m] = TRUE;
		if (!is_precolored(node1)) {
			if (!G_nodeListHas(adjList[m], node2)) {
				G_appendlist(&adjList[m], node2);
				degree[m]++;
			}
		}
		if (!is_precolored(node2)) {
			if (!G_nodeListHas(adjList[n], node1)) {
				G_appendlist(&adjList[n], node1);
				degree[n]++;
			}
		}
	} 
}

static G_nodeList G_union_set(G_nodeList s1, G_nodeList s2) {
	G_nodeList p1 = s1, p2 = s2;
	G_nodeList ret = NULL;
	for (; p1; p1=p1->tail) {
		if (!G_nodeListHas(ret, p1->head)) {
			G_appendlist(&ret, p1->head);
		}
	}
	for (; p2; p2=p2->tail) {
		if (!G_nodeListHas(ret, p2->head)) {
			G_appendlist(&ret, p2->head);
		}
	}
	return ret;
}

static G_nodeList G_differ_set(G_nodeList s1, G_nodeList s2) {
	G_nodeList p1 = s1, p2 = s2;
	G_nodeList ret = NULL;
	for (; p1; p1=p1->tail) {
		G_node n = p1->head;
		if(!G_nodeListHas(s2, n)) {
			G_appendlist(&ret, n);
		}
	}
	return ret;
}

static G_nodeList get_adjacent(G_node node) {
	return G_differ_set(adjList[node->mykey], selectStack);
}

static void makeWorklist(G_nodeList nodes_initial) {
	G_nodeList p = nodes_initial;
	for (; p; p=p->tail) {
		G_node node = p->head;
		if (degree[node->mykey]>=K) {
			G_appendlist(&spillWorklist, node);
		}
		//should add colease support here
		else {
			G_appendlist(&simplifyWorklist, node);
		}
	}
}

static void decrementDegree(G_node node) {
	int d = degree[node->mykey];
	degree[node->mykey]--;
	if (d == K) {
	//	if (!G_nodeListHas(selectStack, node)) return;
		G_deletelist(&spillWorklist, node);
		G_appendlist(&simplifyWorklist, node);
	}
}

static void simplify() {
	if (simplifyWorklist!=NULL) {
		G_node node = G_peeklist(&simplifyWorklist);
		G_pushstack(&selectStack, node);
		G_nodeList l = get_adjacent(node);
		for (; l; l=l->tail) {
			decrementDegree(l->head);
		}
	}
}

static void selectSpill() {
#if 0
	G_node node = G_peeklist(&spillWorklist);
	G_appendlist(&simplifyWorklist, node);
#else
	G_nodeList p = spillWorklist;
	G_node node_degree_max = spillWorklist->head;
	for (; p; p=p->tail) {
		if (degree[p->head->mykey]>=degree[node_degree_max->mykey]) {
			node_degree_max = p->head;
		}
	}
	G_deletelist(&spillWorklist, node_degree_max);
	G_appendlist(&simplifyWorklist, node_degree_max);
#endif
}

extern Temp_map F_getTempMap();

static string get_reg_str(Temp_temp reg) {
	Temp_map temp_map = F_getTempMap();
	return Temp_look(temp_map, reg);
}

static COL_stringList all_colors() {
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
  string fck = temp_precolored(t)?"p":" ";
  if (c!=NULL) printf("%s %s ", c, fck);
  else printf("spilled");
}

static void assignColor() {
	while (selectStack!=NULL) {
		G_node n = G_popstack(&selectStack);
		COL_stringList okColors = all_colors();
		G_nodeList g_nodelist = adjList[n->mykey];
		for (; g_nodelist; g_nodelist=g_nodelist->tail) {
			G_nodeList tmplist = G_union_set(coloredNodes, precolored);
			if (okColors==NULL) break;
			if (G_nodeListHas(tmplist, g_nodelist->head)) {
				string color_str = Temp_look(color, G_nodeInfo(g_nodelist->head));
				String_deletelist(&okColors, color_str);
			}
		}
		if (okColors==NULL) {
			G_appendlist(&spilledNodes, n);
			// printf("spill t%d out\n", ((Temp_temp)n->info)->num);
		}
		else {
			G_appendlist(&coloredNodes, n);
			Temp_enter(color, G_nodeInfo(n), okColors->head);
			Temp_temp t = G_nodeInfo(n);
			// printf("color t%d %s\n", t->num, okColors->head);
		}
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
			G_appendlist(&precolored, node);
			Temp_temp t = G_nodeInfo(node);
			string fuck = Temp_look(initial, G_nodeInfo(node));
			printf("t%d -> %s\n", t->num, fuck);
		}
		else {
			G_appendlist(&g_nodes, node);
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
			add_edge(node, adjs->head);
		}
	}

	makeWorklist(g_nodes);
	// for (i = 0; i<nnodes; i++) {printf("degree[%d] = %d\n", i, degree[i]);}
	
	do {
		if (!G_nodeListIsEmpty(simplifyWorklist)) {
			simplify();
		}
		else {
			selectSpill();
		}
	} while (!G_nodeListIsEmpty(simplifyWorklist) || !G_nodeListIsEmpty(spillWorklist));

	assignColor();
	G_show(stdout, G_nodes(graph), Temp_dumpTempWithName);
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
