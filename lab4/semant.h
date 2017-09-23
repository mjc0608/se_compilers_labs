void SEM_transProg(A_exp exp);

typedef void *Tr_exp;

struct expty {
    Tr_exp exp;
    Ty_ty ty;
};

struct expty expTy(Tr_exp exp, Ty_ty ty);
Ty_ty get_real_ty(Ty_ty ty);

struct expty transVar(S_table venv, S_table tenv, A_var v);
struct expty transExp(S_table venv, S_table tenv, A_exp a);
void transDec(S_table venv, S_table tenv, A_dec d);

Ty_ty transTy(S_table tenv, A_ty ty);
int para_match(S_table venv, S_table tenv, A_expList explist, Ty_tyList tylist, A_exp func);
int ty_match(Ty_ty ty1, Ty_ty ty2);
