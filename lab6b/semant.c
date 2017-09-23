#include <stdio.h>
#include <string.h>
#include "util.h"
#include "errormsg.h"
#include "symbol.h"
#include "absyn.h"
#include "types.h"
#include "env.h"
#include "semant.h"
#include "translate.h"
#include "util.h"

/*Lab4: Your implementation of lab4*/
F_fragList SEM_transProg(A_exp exp){
    S_table venv = E_base_venv();
    S_table tenv = E_base_tenv();
// #ifdef FUCK_THE_LAB
//     frags = F_FragList(F_ProcFrag(NULL, NULL), frags); // fuck the lab
// #endif
    struct expty et = transExp(Tr_outermost(), NULL, venv, tenv, exp);
    Tr_procEntryExit(Tr_outermost(), et.exp, NULL, Temp_namedlabel("tigermain"));
    return Tr_getResult();
}

struct expty expTy(Tr_exp exp, Ty_ty ty) {
    struct expty e;
    e.exp = exp;
    e.ty = ty;
    return e;
}

static U_boolList makeFormals(A_fieldList params) {
	U_boolList head = NULL, tail = NULL;
	A_fieldList p = params;
	for (; p; p = p->tail) {
		if (head) {
			tail->tail = U_BoolList(TRUE, NULL);
			tail = tail->tail;
		} else {
			head = U_BoolList(TRUE, NULL);
			tail = head;
		}
	}
	return head;
}

struct expty transVar(Tr_level level, Tr_exp breakk, S_table venv, S_table tenv, A_var v) {
    switch(v->kind) {
        case A_simpleVar: {
            E_enventry x = S_look(venv, v->u.simple);
            Tr_exp trans = Tr_NoExp();
            if (x && x->kind==E_varEntry) {
                trans = Tr_SimpleVar(x->u.var.access, level);
                return expTy(trans, get_real_ty(x->u.var.ty));
            }
            else {
                EM_error(v->pos, "undefined variable %s", S_name(v->u.simple));
                return expTy(trans, Ty_Int());
            }
            break;
        }
        case A_fieldVar: {
            struct expty left = transVar(level, breakk, venv, tenv, v->u.field.var);
            Tr_exp trans = Tr_NoExp();
            if (left.ty->kind!=Ty_record) {
                EM_error(v->pos, "not a record type");
                return expTy(trans, Ty_Record(NULL));
            }
            else {
                Ty_fieldList field;
                int i = 0;
                for (field=left.ty->u.record; field!=NULL; field=field->tail, i++) {
                    if (field->head->name == v->u.field.sym) {
                        trans = Tr_FieldVar(left.exp, i);
                        return expTy(trans, get_real_ty(field->head->ty));
                    }
                }
                EM_error(v->pos, "field %s doesn't exist", S_name(v->u.field.sym));
                return expTy(trans, Ty_Record(NULL));
            }
            break;
        }
        case A_subscriptVar: {
            struct expty left = transVar(level, breakk, venv, tenv, v->u.subscript.var);
            Tr_exp trans = Tr_NoExp();
            if (left.ty->kind!=Ty_array) {
                EM_error(v->pos, "array type required");
                return expTy(trans, Ty_Int());
            }
            else {
                struct expty exp = transExp(level, breakk, venv, tenv, v->u.subscript.exp);
                if (exp.ty->kind==Ty_int) {
                    trans = Tr_SubscriptVar(left.exp, exp.exp);
                    return expTy(trans, get_real_ty(left.ty->u.array));
                }
                else {
                    EM_error(v->pos, "int required");
                    return expTy(trans, Ty_Int());
                }
            }
            break;
        }
    }
}

struct expty transExp(Tr_level level, Tr_exp breakk, S_table venv, S_table tenv, A_exp a) {
    static S_table loop_venv = NULL;
    if (loop_venv==NULL) loop_venv = E_base_venv();
    Tr_exp trans = Tr_NoExp();
    switch(a->kind) {
        case A_varExp: {
            return transVar(level, breakk, venv, tenv, a->u.var);
            break;
        }
        case A_nilExp: {
            trans = Tr_NilExp();
            return expTy(trans, Ty_Nil());
            break;
        }
        case A_intExp: {
            trans = Tr_IntExp(a->u.intt);
            return expTy(trans, Ty_Int());
            break;
        }
        case A_stringExp: {
// #ifdef FUCK_THE_LAB
//             frags = F_FragList(F_StringFrag(Temp_newlabel(), NULL), frags); // fuck the lab
// #endif
            trans = Tr_StringExp(a->u.stringg, level);
            return expTy(trans, Ty_String());
            break;
        }
        case A_callExp: {
            E_enventry call = S_look(venv, a->u.call.func);
            A_expList args = NULL;
            Tr_expList tr_args = NULL;
            Ty_tyList tylist_head = NULL, tylist = NULL;
            
            for (args=a->u.call.args; args!=NULL; args = args->tail) {
                struct expty arg = transExp(level, breakk, venv, tenv,args->head);
                tr_args = Tr_expList_append(tr_args, arg.exp);
                if (tylist_head==NULL) {
                    tylist_head = Ty_TyList(arg.ty, NULL);
                    tylist = tylist_head;
                }
                else {
                    tylist->tail = Ty_TyList(arg.ty, NULL);
                    tylist = tylist->tail;
                }
            }
            if (!call || call->kind!=E_funEntry) {
                EM_error(a->pos, "undefined function %s", S_name(a->u.call.func));
            }
            else {
                bool needslk = need_staticlink(a->u.call.func);
                trans = Tr_CallExp(call->u.fun.label, call->u.fun.level->parent, level, tr_args, needslk);
                if (!para_match(level, breakk, venv, tenv, tylist_head, call->u.fun.formals, a)) {
                    // EM_error(a->pos, "para type mismatch");
                    return expTy(trans, Ty_Void());
                }
                else {
                    if (call->u.fun.result!=NULL) {
                        return expTy(trans, get_real_ty(call->u.fun.result));
                    }
                    else {
                        return expTy(trans, Ty_Nil());
                    }
                }
            }
            return expTy(trans, Ty_Void());
            break;
        }
        case A_opExp: {
            A_oper oper = a->u.op.oper;
            struct expty left = transExp(level, breakk, venv, tenv, a->u.op.left);
            struct expty right = transExp(level, breakk, venv, tenv, a->u.op.right);
            if (oper==A_plusOp || oper==A_minusOp || oper==A_timesOp || oper==A_divideOp ) {
                if (left.ty->kind!=Ty_int) {
                    EM_error(a->u.op.left->pos, "integer required");
                }
                else if (right.ty->kind!=Ty_int) {
                    EM_error(a->u.op.right->pos, "integer required");
                }
                else {
                    return expTy(Tr_OpExp(oper, left.exp, right.exp), Ty_Int());
                }
                return expTy(NULL, Ty_Int());
            }
            else if (oper == A_eqOp || oper == A_neqOp) {
                if (left.ty->kind==Ty_record && right.ty->kind==Ty_nil) { 
                    trans = Tr_CmpExp(oper, left.exp, right.exp);
                    return expTy(trans, Ty_Int());
                }
                else if (left.ty->kind==Ty_nil && right.ty->kind==Ty_record) {
                    trans = Tr_CmpExp(oper, left.exp, right.exp);
                    return expTy(trans, Ty_Int());
                }
                else {
                    if (left.ty->kind==Ty_int && right.ty->kind==Ty_int) {
                        trans = Tr_CmpExp(oper, left.exp, right.exp);
                        return expTy(trans, Ty_Int());
                    } 
                    else if (left.ty->kind==Ty_string && right.ty->kind==Ty_string) {
                        trans = Tr_EqStringExp(oper, left.exp, right.exp);
                        return expTy(trans, Ty_Int());
                    }
                    else {
                        EM_error(a->pos, "same type required");
                        return expTy(trans, Ty_Int());
                    }
                }
            }
            else {
                if ((left.ty->kind==Ty_int && right.ty->kind==Ty_int) || (left.ty->kind==Ty_string && right.ty->kind==Ty_string)) {
                    trans = Tr_CmpExp(oper, left.exp, right.exp);
                    return expTy(trans, Ty_Int());
                }
                else {
                    EM_error(a->pos, "same type required");
                    return expTy(trans, Ty_Int());
                }
            }
        break;
        }
        case A_recordExp: {/* create a record */
            Ty_ty record = get_real_ty(S_look(tenv, a->u.record.typ));
            if (!record || record->kind!=Ty_record) {
                EM_error(a->pos, "undefined type %s", S_name(a->u.record.typ));
                return expTy(trans, Ty_Record(NULL));
            }
            else {
                A_efieldList efields = a->u.record.fields;
                Ty_fieldList fields = record->u.record;
                int n=0;
                Tr_expList l = NULL;
                // struct expty val;

                while (efields && fields) {
                    struct expty efield_exp = transExp(level, breakk, venv, tenv, efields->head->exp);
                    Ty_ty efield_type = efield_exp.ty;

                    if (efield_type!=get_real_ty(fields->head->ty)) {
                        if (!(efield_type->kind == Ty_nil && get_real_ty(fields->head->ty)->kind==Ty_record)) {
                            EM_error(a->pos, "record field type error");
                            return expTy(trans, Ty_Void());
                        }
                    }

                    n++;
                    // val = transExp(level, breakk, venv, tenv, efields->head->exp);
                    // l = Tr_expList_append(l, efield_exp.exp);
                    l = Tr_ExpList(efield_exp.exp, l);

                    efields = efields->tail;
                    fields = fields->tail;
                }
                if (efields) {
                    EM_error(a->pos, "more record field than dec");
                    return expTy(trans, Ty_Void());
                }
                else if (fields) {
                    EM_error(a->pos, "less record field than dec");
                    return expTy(trans, Ty_Void());
                }

                trans = Tr_RecordExp(n, l);
                return expTy(trans, record);
            }
            break;
        }
        case A_seqExp: {
            struct expty e = expTy(trans, Ty_Void());
            A_expList ep = a->u.seq;
            Tr_expList l = NULL;

            if (ep==NULL) return e;

            while (ep!=NULL) {
                e = transExp(level, breakk, venv, tenv, ep->head);
                l = Tr_expList_append(l, e.exp);
                ep = ep->tail;
            }
            return expTy(Tr_SeqExp(l), e.ty);
            break;
        }
        case A_assignExp: {
            struct expty left = transVar(level, breakk, venv, tenv, a->u.assign.var);
            if (left.ty->kind==Ty_record && left.ty->u.record==NULL) return expTy(NULL, Ty_Void());
            if (left.ty->kind==Ty_int && S_look(loop_venv, a->u.assign.var->u.simple)!=NULL) {
                EM_error(a->pos, "loop variable can't be assigned");
                return expTy(NULL, Ty_Void());
            }
            struct expty right = transExp(level, breakk, venv, tenv, a->u.assign.exp);
            if (!ty_match(left.ty, right.ty)) {
                EM_error(a->pos, "unmatched assign exp");
                return expTy(trans, Ty_Void());
            }
            return expTy(Tr_AssignExp(left.exp, right.exp), Ty_Void());
            break;
        }
        case A_ifExp: {
            struct expty test = transExp(level, breakk, venv, tenv, a->u.iff.test);
            struct expty then = transExp(level, breakk, venv, tenv, a->u.iff.then);
            struct expty elsee = { NULL, NULL };
            if (a->u.iff.elsee!=NULL) {
                elsee = transExp(level, breakk, venv, tenv, a->u.iff.elsee);
                if (test.ty->kind!=Ty_int) {
                    EM_error(a->u.iff.test->pos, "not int");
                }
                if (!ty_match(then.ty, elsee.ty)) {
                    EM_error(a->u.iff.then->pos, "then exp and else exp type mismatch");
                }
                return expTy(Tr_IfExp(test.exp, then.exp, elsee.exp), then.ty);
            }
            else {
                if (test.ty->kind!=Ty_int) {
                    EM_error(a->u.iff.test->pos, "not int");
                }
                if (then.ty->kind!=Ty_void) {
                    EM_error(a->u.iff.then->pos, "if-then exp's body must produce no value");
                }
                return expTy(Tr_IfExp(test.exp, then.exp, elsee.exp), then.ty);
            }
            break;
        }
        case A_whileExp: {
            struct expty test = transExp(level, breakk, venv, tenv, a->u.whilee.test);
            if (test.ty->kind!=Ty_int) {
                EM_error(a->u.whilee.test->pos, "not int");
            }
            Tr_exp done = Tr_DoneExp();
            struct expty body = transExp(level, done, venv, tenv, a->u.whilee.body);
            if (body.ty!=NULL && body.ty->kind!=Ty_void) {
                EM_error(a->u.whilee.test->pos, "while body must produce no value");
            }
            trans = Tr_WhileExp(test.exp, body.exp, done);
            return expTy(trans, Ty_Void());
            break;
        }
        case A_forExp: {
#if 0
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
#else
            A_dec i_dec = A_VarDec(a->pos, a->u.forr.var, S_Symbol("int"), a->u.forr.lo);
            A_dec limit_dec = A_VarDec(a->pos, S_Symbol("limit"), S_Symbol("int"), a->u.forr.hi);
            A_exp i_exp = A_VarExp(a->pos, A_SimpleVar(a->pos, a->u.forr.var));
            A_exp limit_exp = A_VarExp(a->pos, A_SimpleVar(a->pos, S_Symbol("limit")));
            A_exp test_exp = A_OpExp(a->pos, A_leOp, i_exp, limit_exp);
            A_exp increment = A_AssignExp(a->pos, 
                A_SimpleVar(a->pos, a->u.forr.var),
                A_OpExp(a->pos, A_plusOp, i_exp, A_IntExp(a->pos, 1)));
            A_exp let_exp = A_LetExp(a->pos,
                A_DecList(i_dec, A_DecList(limit_dec, NULL)),
                A_WhileExp(a->pos, 
                    test_exp, 
                    A_SeqExp(a->pos,
                        A_ExpList(a->u.forr.body, 
                        A_ExpList(increment, NULL)))));

            struct expty e = transExp(level, breakk, venv, tenv, let_exp);
            return e;

#endif 
        }
        case A_breakExp: {
            if (!breakk) {
                EM_error(a->pos, "should inside loop");
            }
            else {
                trans = Tr_BreakExp(breakk);
            }
            return expTy(trans, Ty_Void());
        }
        case A_letExp: {
            A_decList dec_p;
            struct expty body;
            Tr_expList l = NULL;
            S_beginScope(venv);
            S_beginScope(tenv);
            for (dec_p = a->u.let.decs; dec_p!=NULL; dec_p=dec_p->tail) {
                Tr_exp e = transDec(level, breakk, venv, tenv, dec_p->head);
                l = Tr_expList_append(l, e);
            }
            body = transExp(level, breakk, venv, tenv, a->u.let.body);
            l = Tr_expList_append(l, body.exp);
            S_endScope(venv);
            S_endScope(tenv);

            trans = Tr_SeqExp(l);
            return expTy(trans, body.ty);
            break;
        }
        case A_arrayExp: {
            Ty_ty array = get_real_ty(S_look(tenv, a->u.array.typ));
            struct expty size;
            struct expty init;
            if (array==NULL) {
                EM_error(a->pos, "undefined");
                return expTy(trans, Ty_Array(NULL));
            }
            if (array->kind!=Ty_array) {
                EM_error(a->pos, "array type required");
                return expTy(trans, Ty_Array(NULL));
            }
            size = transExp(level, breakk, venv, tenv, a->u.array.size);
            init = transExp(level, breakk, venv, tenv, a->u.array.init);
            if (size.ty->kind!=Ty_int) {
                EM_error(a->pos, "size should be int");
                return expTy(trans, Ty_Array(NULL));
            }
            if (!ty_match(init.ty, array->u.array)) {
                EM_error(a->pos, "type mismatch");
                return expTy(trans, Ty_Array(NULL));
            }
            trans = Tr_ArrayExp(size.exp, init.exp);
            return expTy(trans, array);
            break;
        }
    }
    return expTy(NULL, NULL);
}

Tr_exp transDec(Tr_level level, Tr_exp breakk, S_table venv, S_table tenv, A_dec d) {
    Tr_access ac;
    switch(d->kind) {
        case A_varDec: {
            struct expty val = transExp(level, breakk, venv, tenv, d->u.var.init);
            ac = Tr_allocLocal(level, d->u.var.escape);
            if (d->u.var.typ==NULL) {
                if (val.ty == NULL) {
                    EM_error(d->pos, "integer required");
                    S_enter(venv, d->u.var.var, E_VarEntry(ac, Ty_Int()));
                    return Tr_AssignExp(Tr_SimpleVar(ac, level), val.exp);
                }
                if (val.ty->kind==Ty_nil) {
                    EM_error(d->pos, "init should not be nil without type specified");
                    S_enter(venv, d->u.var.var, E_VarEntry(ac, Ty_Int()));
                    return Tr_AssignExp(Tr_SimpleVar(ac, level), val.exp);
                }
                else if (val.ty->kind==Ty_void) {
                    EM_error(d->pos, "integer required");
                    return Tr_AssignExp(Tr_SimpleVar(ac, level), val.exp);
                }
                else {
                    S_enter(venv, d->u.var.var, E_VarEntry(ac, val.ty));
                    return Tr_AssignExp(Tr_SimpleVar(ac, level), val.exp);
                }
            }
            else {
                Ty_ty ty = S_look(tenv, d->u.var.typ);
                if (ty==NULL) {
                    EM_error(d->pos, "type %s undefined", S_name(d->u.var.typ));
                    return Tr_AssignExp(Tr_SimpleVar(ac, level), val.exp);
                }
                if (!ty_match(ty, val.ty)) {
                    EM_error(d->pos, "type mismatch");
                    S_enter(venv, d->u.var.var, E_VarEntry(ac, ty));
                    return Tr_AssignExp(Tr_SimpleVar(ac, level), val.exp);
                }
                else {
                    S_enter(venv, d->u.var.var, E_VarEntry(ac, ty));
                }
            }
            return Tr_AssignExp(Tr_SimpleVar(ac, level), val.exp);
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
                Temp_label func_label;
                Tr_level func_level;

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
                func_label = Temp_namedlabel(S_name(fp->head->name));
                func_level = Tr_newLevel(level, func_label, makeFormals(fp->head->params));
                E_enventry funentry = E_FunEntry(func_level, func_label, tys_h, ret_ty);
                S_enter(venv, fp->head->name, funentry);
                S_enter(block_venv, fp->head->name, funentry);
            }
            for (fp=d->u.function; fp!=NULL; fp=fp->tail) {
                Ty_ty ret_ty;
                struct expty real_ret_ty_exp;
                Ty_tyList tys = NULL, tys_h = NULL;
                Tr_accessList ac_list = NULL, ac_list_tmp = NULL;

                E_enventry f = S_look(block_venv, fp->head->name);
                tys_h = f->u.fun.formals;
                ret_ty = f->u.fun.result;
                
                S_beginScope(venv);
                ac_list = Tr_formals(f->u.fun.level);
                ac_list_tmp = ac_list;
                for (field_p=fp->head->params, tylist_p=tys_h; 
                    field_p!=NULL && tylist_p!=NULL && ac_list_tmp!=NULL; 
                    field_p=field_p->tail, tylist_p=tylist_p->tail, ac_list_tmp=ac_list_tmp->tail) {
                    S_enter(venv, field_p->head->name, E_VarEntry(ac_list_tmp->head, tylist_p->head));
                }
                real_ret_ty_exp = transExp(f->u.fun.level, breakk, venv, tenv, fp->head->body);
                if (!ty_match(ret_ty, real_ret_ty_exp.ty)) {
                    EM_error(fp->head->pos, "procedure returns value");
                }
                Tr_procEntryExit(f->u.fun.level, real_ret_ty_exp.exp, ac_list, f->u.fun.label);
                S_endScope(venv);
            }
            return Tr_NoExp();
            break;
        }
        case A_typeDec: {
            A_nametyList namety_p;
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
                
                t_name->u.name.ty = t;
                if (is_illegal_cycle(tenv, t_name)) {
                    EM_error(d->pos, "illegal type cycle");
                }
            }
            return Tr_NoExp();
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
                EM_error(ty->pos, "undefined type %s", S_name(ty->u.array));
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

int is_illegal_cycle(S_table tenv, Ty_ty ty) {
    Ty_ty t = ty;
    while (t && t->kind == Ty_name) {
        if (t->u.name.ty) {
            t = t->u.name.ty;
            if (t->kind == Ty_name && strcmp(S_name(ty->u.name.sym), S_name(t->u.name.sym))==0) {
                return 1; /* cycle */
            }
        }
        else {
            t = S_look(tenv, t->u.name.sym);
            if (t && t->kind == Ty_name && t->u.name.ty == NULL) break;
        }
    }
    return 0;
}

int para_match(Tr_level level, Tr_exp breakk, S_table venv, S_table tenv, Ty_tyList call_tylist, Ty_tyList tylist, A_exp func) {
    Ty_tyList ctp = call_tylist;
    Ty_tyList tp = tylist;

    while (ctp!=NULL && tp!=NULL) {
        if (!ty_match(ctp->head, tp->head)) {
            EM_error(func->pos, "para type mismatch");
            return 0;
        }
        ctp = ctp->tail;
        tp = tp->tail;
    }

    if (ctp!=NULL && tp==NULL) {
        EM_error(func->pos, "too many params in function %s", S_name(func->u.call.func));
        return 0;
    }
    else if (ctp==NULL && tp!=NULL) {
        EM_error(func->pos, "too less params in function %s", S_name(func->u.call.func));
        return 0;
    }
    else {
        return 1;
    }
}

int ty_match(Ty_ty ty1, Ty_ty ty2) {
    if ((get_real_ty(ty1)->kind==Ty_int && get_real_ty(ty2)->kind==Ty_int) 
        || (get_real_ty(ty1)->kind==Ty_string && get_real_ty(ty2)->kind==Ty_string))
        return 1;
    else if ((get_real_ty(ty1)->kind==Ty_nil && get_real_ty(ty2)->kind==Ty_record) 
        || (get_real_ty(ty1)->kind==Ty_record && get_real_ty(ty2)->kind==Ty_nil))
        return 1;
    else {
        Ty_ty t1 = get_real_ty(ty1);
        Ty_ty t2 = get_real_ty(ty2);
        if (t1==t2) return 1;
    }
    return 0;
}
