#include "frame.h"
#include "translate.h"

F_fragList SEM_transProg(A_exp exp);

struct expty {
    Tr_exp exp;
    Ty_ty ty;
};

struct expty expTy(Tr_exp exp, Ty_ty ty);
Ty_ty get_real_ty(Ty_ty ty);
int is_illegal_cycle(S_table tenv, Ty_ty ty);

struct expty transVar(Tr_level level, Tr_exp breakk, S_table venv, S_table tenv, A_var v);
struct expty transExp(Tr_level level, Tr_exp breakk, S_table venv, S_table tenv, A_exp a);
Tr_exp transDec(Tr_level level, Tr_exp breakk, S_table venv, S_table tenv, A_dec d);

Ty_ty transTy(S_table tenv, A_ty ty);
int para_match(Tr_level level, Tr_exp breakk, S_table venv, S_table tenv, Ty_tyList call_tylist, Ty_tyList tylist, A_exp func);
int ty_match(Ty_ty ty1, Ty_ty ty2);
