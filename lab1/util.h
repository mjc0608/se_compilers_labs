#ifndef UTIL_H
#define UTIL_H
#include <assert.h>

typedef char *string;
typedef char bool;

#define TRUE 1
#define FALSE 0

void *checked_malloc(int);
string String(char *);

typedef struct U_boolList_ *U_boolList;
struct U_boolList_ {bool head; U_boolList tail;};
U_boolList U_BoolList(bool head, U_boolList tail);


typedef struct table_item_ *table_item;
typedef struct table_ *table;

struct table_item_ {
    string id;
    int val;
    table_item next;
};

struct table_ {
    table_item first;
};

table create_table();
void table_add(table t, string id, int val);
table_item table_find(table t, string id);


#endif
