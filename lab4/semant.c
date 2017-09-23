#include <stdio.h>
#include "util.h"
#include "errormsg.h"
#include "symbol.h"
#include "absyn.h"
#include "types.h"
#include "env.h"
#include "semant.h"

/*Lab4: Your implementation of lab4*/
void SEM_transProg(A_exp exp){
    S_table venv = E_base_venv();
    S_table tenv = E_base_tenv();
    struct expty et = transExp(venv, tenv, exp);
}

struct expty expTy(Tr_exp exp, Ty_ty ty) {
    struct expty e;
    e.exp = exp;
    e.ty = ty;
    return e;
}

struct expty transVar(S_table venv, S_table tenv, A_var v) {
    switch(v->kind) {
        case A_simpleVar: {
            E_enventry x = S_look(venv, v->u.simple);
            if (x && x->kind==E_varEntry)
                return expTy(NULL, get_real_ty(x->u.var.ty));
            else {
                EM_error(v->pos, "undefined variable %s", S_name(v->u.simple));
                return expTy(NULL, Ty_Int());
            }
            break;
        }
        case A_fieldVar: {
            struct expty left = transVar(venv, tenv, v->u.field.var);
            if (left.ty->kind!=Ty_record) {
                EM_error(v->pos, "not a record type");
                return expTy(NULL, Ty_Record(NULL));
            }
            else {
                Ty_fieldList field;
                for (field=left.ty->u.record; field!=NULL; field=field->tail) {
                    if (field->head->name == v->u.field.sym) {
                        return expTy(NULL, get_real_ty(field->head->ty));
                    }
                }
                EM_error(v->pos, "field %s doesn't exist", S_name(v->u.field.sym));
                return expTy(NULL, Ty_Record(NULL));
            }
            break;
        }
        case A_subscriptVar: {
            struct expty left = transVar(venv, tenv, v->u.subscript.var);
            if (left.ty->kind!=Ty_array) {
                EM_error(v->pos, "array type required");
                return expTy(NULL, Ty_Array(NULL));
            }
            else {
                struct expty exp = transExp(venv, tenv, v->u.subscript.exp);
                if (exp.ty->kind==Ty_int) {
                    return expTy(NULL, get_real_ty(left.ty->u.array));
                }
                else {
                    EM_error(v->pos, "int required");
                    return expTy(NULL, Ty_Array(NULL));
                }
            }
            break;
        }
    }
}

struct expty transExp(S_table venv, S_table tenv, A_exp a) {
    static S_table loop_venv = NULL;
    if (loop_venv==NULL) loop_venv = E_base_venv();
    switch(a->kind) {
        case A_varExp: {
            return transVar(venv, tenv, a->u.var);
            break;
        }
        case A_nilExp: {
            return expTy(NULL, Ty_Nil());
            break;
        }
        case A_intExp: {
            return expTy(NULL, Ty_Int());
            break;
        }
        case A_stringExp: {
            return expTy(NULL, Ty_String());
            break;
        }
        case A_callExp: {
            E_enventry call = S_look(venv, a->u.call.func);
            if (!call || call->kind!=E_funEntry) {
                EM_error(a->pos, "undefined function %s", S_name(a->u.call.func));
            }
            else {
                if (!para_match(venv, tenv, a->u.call.args, call->u.fun.formals, a)) {
                    // EM_error(a->pos, "para type mismatch");
                    return expTy(NULL, Ty_Void());
                }
                else {
                    if (call->u.fun.result!=NULL) {
                        return expTy(NULL, get_real_ty(call->u.fun.result));
                    }
                    else {
                        return expTy(NULL, Ty_Nil());
                    }
                }
            }
            break;
        }
        case A_opExp: {
            A_oper oper = a->u.op.oper;
            struct expty left = transExp(venv, tenv, a->u.op.left);
            struct expty right = transExp(venv, tenv, a->u.op.right);
            if (oper==A_plusOp || oper==A_minusOp || oper==A_timesOp || oper==A_divideOp ) {
                if (left.ty->kind!=Ty_int)
                    EM_error(a->u.op.left->pos, "integer required");
                if (right.ty->kind!=Ty_int)
                    EM_error(a->u.op.right->pos, "integer required");
                return expTy(NULL, Ty_Int());
            }
            else if (oper == A_eqOp || oper == A_neqOp) {
                if (left.ty->kind==Ty_record && right.ty->kind==Ty_nil) 
                    return expTy(NULL, Ty_Int());
                else if (left.ty->kind==Ty_nil && right.ty->kind==Ty_record) 
                    return expTy(NULL, Ty_Int());
                else {
                    if ((left.ty->kind==Ty_int && right.ty->kind==Ty_int) || (left.ty->kind==Ty_string && right.ty->kind==Ty_string)) {
                        return expTy(NULL, Ty_Int());
                    }
                    else {
                        EM_error(a->pos, "same type required");
                        return expTy(NULL, Ty_Int());
                    }
                }
            }
            else {
                if ((left.ty->kind==Ty_int && right.ty->kind==Ty_int) || (left.ty->kind==Ty_string && right.ty->kind==Ty_string)) {
                    return expTy(NULL, Ty_Int());
                }
                else {
                    EM_error(a->pos, "same type required");
                    return expTy(NULL, Ty_Int());
                }
            }
        break;
        }
        case A_recordExp: {
            Ty_ty record = get_real_ty(S_look(tenv, a->u.record.typ));
            if (!record || record->kind!=Ty_record) {
                EM_error(a->pos, "undefined type %s", S_name(a->u.record.typ));
                return expTy(NULL, Ty_Record(NULL));
            }
            else {
                return expTy(NULL, record);
            }
            break;
        }
        case A_seqExp: {
            struct expty e = expTy(NULL, Ty_Void());
            A_expList ep = a->u.seq;

            while (ep!=NULL) {
                e = transExp(venv, tenv, ep->head);
                ep = ep->tail;
            }
            return e;
            break;
        }
        case A_assignExp: {
            struct expty left = transVar(venv, tenv, a->u.assign.var);
            if (left.ty->kind==Ty_record && left.ty->u.record==NULL) return expTy(NULL, Ty_Void());
            if (left.ty->kind==Ty_int && S_look(loop_venv, a->u.assign.var->u.simple)!=NULL) {
                EM_error(a->pos, "loop variable can't be assigned");
                return expTy(NULL, Ty_Void());
            }
            struct expty right = transExp(venv, tenv, a->u.assign.exp);
            if (!ty_match(left.ty, right.ty)) {
                EM_error(a->pos, "unmatched assign exp");
            }
            return expTy(NULL, Ty_Void());
            break;
        }
        case A_ifExp: {
            struct expty test = transExp(venv, tenv, a->u.iff.test);
            struct expty then = transExp(venv, tenv, a->u.iff.then);
            struct expty elsee;
            if (a->u.iff.elsee!=NULL) {
                elsee = transExp(venv, tenv, a->u.iff.elsee);
                if (test.ty->kind!=Ty_int) {
                    EM_error(a->u.iff.test->pos, "not int");
                }
                if (!ty_match(then.ty, elsee.ty)) {
                    EM_error(a->u.iff.then->pos, "then exp and else exp type mismatch");
                }
                return expTy(NULL, then.ty);
            }
            else {
                if (test.ty->kind!=Ty_int) {
                    EM_error(a->u.iff.test->pos, "not int");
                }
                if (then.ty->kind!=Ty_void) {
                    EM_error(a->u.iff.then->pos, "if-then exp's body must produce no value");
                }
                return expTy(NULL, then.ty);
            }
            break;
        }
        case A_whileExp: {
            struct expty test = transExp(venv, tenv, a->u.whilee.test);
            if (test.ty->kind!=Ty_int) {
                EM_error(a->u.whilee.test->pos, "not int");
            }
            struct expty body = transExp(venv, tenv, a->u.whilee.body);
            if (body.ty!=NULL && body.ty->kind!=Ty_void) {
                EM_error(a->u.whilee.test->pos, "while body must produce no value");
            }
            return expTy(NULL, Ty_Void());
            break;
        }
        case A_forExp: {
            struct expty lo = transExp(venv, tenv, a->u.forr.lo);
            struct expty hi = transExp(venv, tenv, a->u.forr.hi);
            struct expty body;
            if (lo.ty->kind!=Ty_int) {
                EM_error(a->u.forr.lo->pos, "for exp's range type is not integer");
            }
            if (hi.ty->kind!=Ty_int) {
                EM_error(a->u.forr.hi->pos, "for exp's range type is not integer");
            }
            
            S_beginScope(venv);
            S_beginScope(loop_venv);
            transDec(venv, tenv, A_VarDec(a->pos, a->u.forr.var, S_Symbol("int"), a->u.forr.lo));
            transDec(loop_venv, tenv, A_VarDec(a->pos, a->u.forr.var, S_Symbol("int"), a->u.forr.lo));
            body = transExp(venv, tenv, a->u.forr.body);
            S_endScope(venv);
            S_endScope(loop_venv);

            return expTy(NULL, Ty_Void());
            break;
        }
        case A_breakExp: {
            return expTy(NULL, Ty_Void());
        }
        case A_letExp: {
            A_decList dec_p;
            struct expty body;
            S_beginScope(venv);
            S_beginScope(tenv);
            for (dec_p = a->u.let.decs; dec_p!=NULL; dec_p=dec_p->tail) {
                transDec(venv, tenv, dec_p->head);
            }
            body = transExp(venv, tenv, a->u.let.body);
            S_endScope(venv);
            S_endScope(tenv);
            return body;
            break;
        }
        case A_arrayExp: {
            Ty_ty array = get_real_ty(S_look(tenv, a->u.array.typ));
            struct expty size;
            struct expty init;
            if (array==NULL) {
                EM_error(a->pos, "undefined");
                return expTy(NULL, Ty_Array(NULL));
            }
            if (array->kind!=Ty_array) {
                EM_error(a->pos, "array type required");
                return expTy(NULL, Ty_Array(NULL));
            }
            size = transExp(venv, tenv, a->u.array.size);
            init = transExp(venv, tenv, a->u.array.init);
            if (size.ty->kind!=Ty_int) {
                EM_error(a->pos, "size should be int");
                return expTy(NULL, Ty_Array(NULL));
            }
            if (!ty_match(init.ty, array->u.array)) {
                EM_error(a->pos, "type mismatch");
                return expTy(NULL, Ty_Array(NULL));
            }
            return expTy(NULL, array);
            break;
        }
    }
    return expTy(NULL, NULL);
}

void transDec(S_table venv, S_table tenv, A_dec d) {
    switch(d->kind) {
        case A_varDec: {
            struct expty val = transExp(venv, tenv, d->u.var.init);
            if (d->u.var.typ==NULL) {
                if (val.ty == NULL) {
                    EM_error(d->pos, "integer required");
                    S_enter(venv, d->u.var.var, Ty_Int());
                    return;
                }
                if (val.ty->kind==Ty_nil) {
                    EM_error(d->pos, "init should not be nil without type specified");
                    S_enter(venv, d->u.var.var, Ty_Int());
                    return;
                }
                else if (val.ty->kind==Ty_void) {
                    EM_error(d->pos, "integer required");
                    return;
                }
                else {
                    S_enter(venv, d->u.var.var, E_VarEntry(val.ty));
                    return;
                }
            }
            else {
                Ty_ty ty = S_look(tenv, d->u.var.typ);
                if (ty==NULL) {
                    EM_error(d->pos, "type %s undefined", S_name(d->u.var.typ));
                    return;
                }
                if (!ty_match(ty, val.ty)) {
                    EM_error(d->pos, "type mismatch");
                    S_enter(venv, d->u.var.var, E_VarEntry(ty));
                    return;
                }
                else {
                    S_enter(venv, d->u.var.var, E_VarEntry(ty));
                }
            }
            break;
        }
        case A_functionDec: {
            A_fundecList fp;
            A_fieldList field_p;
            Ty_tyList tylist_p;
            S_table block_venv = E_base_venv();

            for (fp=d->u.function; fp!=NULL; fp=fp->tail) {
                Ty_ty ret_ty;
                struct expty real_ret_ty_exp;
                Ty_tyList tys = NULL, tys_h = NULL;

                if (S_look(block_venv, fp->head->name)!=NULL) {
                    EM_error(d->pos, "two functions have the same name");
                    continue;
                }
                if (fp->head->result!=NULL) {
                    ret_ty = S_look(tenv, fp->head->result);
                    if (ret_ty==NULL) {
                        EM_error(fp->head->pos, "undefined type");
                        ret_ty = Ty_Void();
                    }
                }
                else {
                    ret_ty = Ty_Void();
                }
                for (field_p=fp->head->params; field_p!=NULL; field_p=field_p->tail) {
                    Ty_ty t = S_look(tenv, field_p->head->typ);
                    if (t==NULL) {
                        EM_error(field_p->head->pos, "undefined type");
                        t = Ty_Int();
                    }
                    if (tys==NULL) {
                        tys = Ty_TyList(t, NULL);
                        tys_h = tys;
                    }
                    else {
                        tys->tail = Ty_TyList(t, NULL);
                        tys = tys->tail;
                    }
                }
                S_enter(venv, fp->head->name, E_FunEntry(tys_h, ret_ty));
                S_enter(block_venv, fp->head->name, E_FunEntry(tys_h, ret_ty));
                S_beginScope(venv);
                for (field_p=fp->head->params, tylist_p=tys_h; field_p!=NULL && tylist_p!=NULL; field_p=field_p->tail, tylist_p=tylist_p->tail) {
                    S_enter(venv, field_p->head->name, E_VarEntry(tylist_p->head));
                }
                real_ret_ty_exp = transExp(venv, tenv, fp->head->body);
                if (!ty_match(ret_ty, real_ret_ty_exp.ty)) {
                    EM_error(fp->head->pos, "procedure returns value");
                }
                S_endScope(venv);
            }
            break;
        }
        case A_typeDec: {
            A_nametyList namety_p;
            int cycle_tag = 1;
            S_table block_tenv = E_base_tenv();

            for (namety_p=d->u.type; namety_p!=NULL; namety_p=namety_p->tail) {
                if (S_look(block_tenv, namety_p->head->name)!=NULL) {
                    EM_error(d->pos, "two types have the same name");
                }
                else {
                    S_enter(tenv, namety_p->head->name, Ty_Name(namety_p->head->name, NULL));
                    S_enter(block_tenv, namety_p->head->name, Ty_Name(namety_p->head->name, NULL));
                }
            }
            for (namety_p=d->u.type; namety_p!=NULL; namety_p=namety_p->tail) {
                Ty_ty t = transTy(tenv, namety_p->head->ty);
                Ty_ty t_name = S_look(tenv, namety_p->head->name);
                /* the code below contains bugs */
                if (cycle_tag) {
                    if (t->kind==Ty_name) {
                        cycle_tag = 0;
                    }
                }
                t_name->u.name.ty = t;
            }
            if (cycle_tag==0) {
                EM_error(d->pos, "illegal type cycle");
            }
            break;
        }
    }
}
Ty_ty transTy(S_table tenv, A_ty ty) {
    switch(ty->kind) {
        case A_nameTy: {
            Ty_ty t = S_look(tenv, ty->u.name);
            if (t==NULL) {
                EM_error(ty->pos, "undefined type %s", S_name(ty->u.name));
                return Ty_Int();
            }
            return t;
        }
        case A_recordTy: {
            A_fieldList fp;
            Ty_fieldList fieldlist = NULL, fieldlist_h = NULL;
            for (fp=ty->u.record; fp!=NULL; fp=fp->tail) {
                Ty_ty t = S_look(tenv, fp->head->typ);
                if (t==NULL) {
                    EM_error(ty->pos, "undefined type %s", S_name(fp->head->typ));
                }
                if (fieldlist==NULL) {
                    Ty_field field = Ty_Field(fp->head->name, t);
                    fieldlist = Ty_FieldList(field, NULL);
                    fieldlist_h = fieldlist;
                }
                else {
                    Ty_field field = Ty_Field(fp->head->name, t);
                    fieldlist->tail = Ty_FieldList(field, NULL);
                    fieldlist=fieldlist->tail;
                }
            }
            return Ty_Record(fieldlist_h);
        }
        case A_arrayTy: {
            Ty_ty t = S_look(tenv, ty->u.array);
            if (t==NULL) {
                EM_error(ty->pos, "undefined type %s", S_name(ty->u.array));\
                return Ty_Array(NULL);
            }
            return Ty_Array(t);
        }
    }
}

Ty_ty get_real_ty(Ty_ty ty) {
    if (!ty) return ty;
    if (ty->kind!=Ty_name) return ty;
    else return get_real_ty(ty->u.name.ty);
}

int para_match(S_table venv, S_table tenv, A_expList explist, Ty_tyList tylist, A_exp func) {
    A_expList ep = explist;
    Ty_tyList tp = tylist;

    while (ep!=NULL && tp!=NULL) {
        struct expty e = transExp(venv, tenv, ep->head);
        if (!ty_match(e.ty, tp->head)) {
            EM_error(func->pos, "para type mismatch");
            return 0;
        }
        ep = ep->tail;
        tp = tp->tail;
    }

    if (ep!=NULL && tp==NULL) {
        EM_error(func->pos, "too many params in function %s", S_name(func->u.call.func));
        return 0;
    }
    else if (ep==NULL && tp!=NULL) {
        EM_error(func->pos, "too less params in function %s", S_name(func->u.call.func));
        return 0;
    }
    else {
        return 1;
    }
}
int ty_match(Ty_ty ty1, Ty_ty ty2) {
    if ((ty1->kind==Ty_int && ty2->kind==Ty_int) || (ty1->kind==Ty_string && ty2->kind==Ty_string))
        return 1;
    else if ((ty1->kind==Ty_nil && ty2->kind==Ty_record) || (ty1->kind==Ty_record && ty2->kind==Ty_nil))
        return 1;
    else {
        Ty_ty t1 = get_real_ty(ty1);
        Ty_ty t2 = get_real_ty(ty2);
        if (t1==t2) return 1;
    }
    return 0;
}
