#ifndef UTIL_H
#define UTIL_H
#include <assert.h>
#include <stdarg.h>

typedef char *string;
typedef char bool;

#define TRUE 1
#define FALSE 0

void *checked_malloc(int);
string String(char *);
string String_fmt(char *, ...);
string String_toPut(char *);

typedef struct U_boolList_ *U_boolList;
struct U_boolList_ {bool head; U_boolList tail;};
U_boolList U_BoolList(bool head, U_boolList tail);
#endif
