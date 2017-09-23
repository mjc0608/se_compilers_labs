/*
 * color.h - Data structures and function prototypes for coloring algorithm
 *             to determine register allocation.
 */

#ifndef COLOR_H
#define COLOR_H

#include "liveness.h"

struct COL_result {Temp_map coloring; Temp_tempList spills;};
struct COL_result COL_color(struct Live_graph ig, Temp_map initial, Temp_tempList regs);

typedef struct COL_stringList_ *COL_stringList;
struct COL_stringList_ {
    string head;
    COL_stringList tail;
};
COL_stringList COL_StringList(string head, COL_stringList tail);

typedef enum { 
    PRECOLORED, 
    SIMPLIFYWORKLIST, 
    FREEZEWORKLIST, 
    SPILLWORKLIST, 
    SPILLEDNODES, 
    COALESCEDNODES, 
    COLOREDNODES, 
    SELECTSTACK,
    COLORDEFAULT
} color_kind;

typedef enum{ 
    COALESCEDMOVES, 
    CONSTRAINTMOVES, 
    FROZENMOVES, 
    WORKLISTMOVES, 
    ACTIVEMOVES ,
    MOVEDEFAULT
} move_kind;




#endif