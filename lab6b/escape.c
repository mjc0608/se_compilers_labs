#include "util.h"
#include "symbol.h" 
#include "absyn.h"  
#include "escape.h"
#include <stdlib.h>
#include <stdio.h>
#include "table.h"

Esc_Binding Esc_newBinding(int depth, bool *escape) {
    Esc_Binding binding = checked_malloc(sizeof(*binding));
    binding->depth = depth;
    binding->escape = escape;
    return binding;
}

static void traverseExp(S_table env, int depth, A_exp a);
static void traverseDec(S_table env, int depth, A_dec d);
static void traverseVar(S_table env, int depth, A_var v);

static void traverseDec(S_table env, int depth, A_dec d) {
    switch(d->kind) {
        case A_varDec: {
            S_symbol var = d->u.var.var;
            A_exp init = d->u.var.init;
            traverseExp(env, depth, init);
            S_enter(env, var, Esc_newBinding(depth, &d->u.var.escape));
            break;
        }
        case A_functionDec: {
            A_fundecList fundeclist = d->u.function;
            for (; fundeclist; fundeclist=fundeclist->tail) {
                A_fundec fundec = fundeclist->head;
                A_fieldList fields = fundec->params;
                S_beginScope(env);
                traverseExp(env, depth+1, fundec->body);
                S_endScope(env);
            }
            break;
        }  
        default:
            break;
    }
}

static void traverseVar(S_table env, int depth, A_var v) {
    switch(v->kind) {
        case A_simpleVar: {
            Esc_Binding binding = S_look(env, v->u.simple);
            if (binding && binding->depth!=depth) {
                *(binding->escape) = TRUE;
            }
            break;
        }
        case A_fieldVar: {
            traverseVar(env, depth, v->u.field.var);
            break;
        }
        case A_subscriptVar: {
            traverseVar(env, depth, v->u.subscript.var);
            traverseExp(env, depth, v->u.subscript.exp);
            break;
        }
    }
}

static void traverseExp(S_table env, int depth, A_exp a) {
    switch(a->kind) {
        case A_varExp: {
            traverseVar(env, depth, a->u.var);
            break;
        }
        case A_callExp: {
            A_expList args = a->u.call.args;
            for (; args; args=args->tail) {
                traverseExp(env, depth, args->head);
            }
            break;
        }
        case A_opExp: {
            traverseExp(env, depth, a->u.op.left);
            traverseExp(env, depth, a->u.op.right);
            break;
        }
        case A_recordExp: {
            A_efieldList efields = a->u.record.fields;
            for (; efields; efields=efields->tail) {
                traverseExp(env, depth, efields->head->exp);
            }
            break;
        }
        case A_seqExp: {
            A_expList exps = a->u.seq;
            for (; exps; exps=exps->tail) {
                traverseExp(env, depth, exps->head);
            }
            break;
        }
        case A_assignExp: {
            traverseVar(env, depth, a->u.assign.var);
            traverseExp(env, depth, a->u.assign.exp);
            break;
        }
        case A_ifExp: {
            traverseExp(env, depth, a->u.iff.test);
            traverseExp(env, depth, a->u.iff.then);
            if (a->u.iff.elsee!=NULL)
                traverseExp(env, depth, a->u.iff.elsee);
            break;
        }
        case A_whileExp: {
            traverseExp(env, depth, a->u.whilee.test);
            traverseExp(env, depth, a->u.whilee.body);
            break;
        }
        case A_forExp: {
            S_enter(env, a->u.forr.var, Esc_newBinding(depth, &a->u.forr.escape));
            traverseExp(env, depth, a->u.forr.lo);
            traverseExp(env, depth, a->u.forr.hi);
            traverseExp(env, depth, a->u.forr.body);
            break;
        }
        case A_letExp: {
            A_decList declist = a->u.let.decs;
            for (; declist; declist=declist->tail) {
                traverseDec(env, depth, declist->head);
            }
            traverseExp(env, depth, a->u.let.body);
            break;
        }
        case A_arrayExp: {
            traverseExp(env, depth, a->u.array.size);
            traverseExp(env, depth, a->u.array.init);
            break;
        }
        default:
            break;
    }
}

void Esc_findEscape(A_exp exp) {
    S_table escape_env = S_empty();
    traverseExp(escape_env, 0, exp);
}
