#ifndef ESCAPE_H
#define ESCAPE_H

void Esc_findEscape(A_exp exp);

typedef struct Esc_Binding_ *Esc_Binding;
struct Esc_Binding_ {
    int depth;
    bool *escape;
};

Esc_Binding Esc_newBinding(int depth, bool *escape);

#endif
