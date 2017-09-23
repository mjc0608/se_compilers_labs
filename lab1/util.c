/*
 * util.c - commonly used utility functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
void *checked_malloc(int len)
{void *p = malloc(len);
 if (!p) {
    fprintf(stderr,"\nRan out of memory!\n");
    exit(1);
 }
 return p;
}

string String(char *s)
{string p = checked_malloc(strlen(s)+1);
 strcpy(p,s);
 return p;
}

U_boolList U_BoolList(bool head, U_boolList tail)
{ U_boolList list = checked_malloc(sizeof(*list));
  list->head = head;
  list->tail = tail;
  return list;
}

table create_table() {
    table t = checked_malloc(sizeof *t);
    t->first=NULL;
    return t;
}

void table_add(table t, string id, int val) {
    table_item ti = checked_malloc(sizeof *ti);
    ti->id = id;
    ti->val = val;
    ti->next = t->first;
    t->first=ti;
}

table_item table_find(table t, string id) {
    table_item iter=t->first;
    while (iter != NULL) {
        if (strcmp(id, iter->id)==0) {
            return iter;
        }
        iter = iter->next;
    }
    return NULL;
}
