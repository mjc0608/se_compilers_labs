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

string String_fmt(char *s, ...) {
  char *ret = malloc(128);
  va_list vp;
  va_start(vp, s);
  vsprintf(ret, s, vp);
  va_end(vp);
  return ret;
}

string String_toPut(char *s) {
  int len = strlen(s);
  char *ret = malloc(2*len+1);
  int i = 0, p = 0;
  for (i=0; i<len; i++) {
    if (s[i]!='\n' && s[i]!='\t') {
      ret[p]=s[i];
      p++;
    }
    else if (s[i]=='\n') {
      ret[p]='\\';
      ret[p+1]='n';
      p+=2;
    }
    else {
      ret[p]='\\';
      ret[p+1]='t';
      p+=2;
    }
  }
  ret[p]='\0';
  return ret;
}

U_boolList U_BoolList(bool head, U_boolList tail)
{ U_boolList list = checked_malloc(sizeof(*list));
  list->head = head;
  list->tail = tail;
  return list;
}
