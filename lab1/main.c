/* This file is not complete.  You should fill it in with your
   solution to the programming exercise. */
#include <stdio.h>
#include "prog1.h"
#include "slp.h"

int maxargs(A_stm stm);
int maxargs_exp(A_exp exp);
int maxargs_exps(A_expList exps);
void interp(A_stm stm);
void interp_stm(table t, A_stm stm);
int interp_exp(table t, A_exp exp);
int interp_print(table t, A_expList exps);

/*
 *Please don't modify the main() function
 */
int main()
{
	int args;

	printf("prog\n");
	args = maxargs(prog());
	printf("args: %d\n",args);
	interp(prog());

	printf("prog_prog\n");
	args = maxargs(prog_prog());
	printf("args: %d\n",args);
	interp(prog_prog());

	printf("right_prog\n");
	args = maxargs(right_prog());
	printf("args: %d\n",args);
	interp(right_prog());

	return 0;
}

int max(int a, int b) {
    return a>b?a:b;
}

int maxargs_exp(A_exp exp) {
    if (exp->kind == A_idExp || exp->kind ==A_numExp) {
        return 0;
    }
    else if(exp->kind == A_opExp) {
        return max(maxargs_exp(exp->u.op.left), maxargs_exp(exp->u.op.right));
    }
    else {
        return max(maxargs(exp->u.eseq.stm), maxargs_exp(exp->u.eseq.exp));
    }
}

int maxargs_exps(A_expList exps) {
    int args_cnt=0;
    int max_substm_cnt=0;
    A_expList exps_iter=exps;
    while (exps_iter->kind != A_lastExpList) {
        args_cnt++;
        if (exps_iter->u.pair.head->kind == A_eseqExp
                || exps_iter->u.pair.head->kind == A_opExp) {
            max_substm_cnt = max(max_substm_cnt, maxargs_exp(exps_iter->u.pair.head));
        }
        exps_iter=exps_iter->u.pair.tail;
    }
    args_cnt++;
    if (exps_iter->u.pair.head->kind == A_eseqExp
            || exps_iter->u.pair.head-> kind == A_opExp) {
        max_substm_cnt = max(max_substm_cnt, maxargs_exp(exps_iter->u.pair.head));
    }
    return max(args_cnt, max_substm_cnt);
}

int maxargs(A_stm stm) {
    if (stm->kind == A_compoundStm) {
        return max(maxargs(stm->u.compound.stm1), maxargs(stm->u.compound.stm2));
    }
    else if (stm->kind == A_assignStm) {
        return maxargs_exp(stm->u.assign.exp);
    }
    else {
        return maxargs_exps(stm->u.print.exps);
    }
}

void interp(A_stm stm){
    table t = create_table();
    interp_stm(t, stm);
}

void interp_stm(table t, A_stm stm) {
    switch(stm->kind) {
        case A_compoundStm:
            interp_stm(t, stm->u.compound.stm1);
            interp_stm(t, stm->u.compound.stm2);
            break;
        case A_assignStm:
            table_add(t, stm->u.assign.id, interp_exp(t, stm->u.assign.exp));
            break;
        case A_printStm:
            interp_print(t, stm->u.print.exps);
            break;
    }
}

int interp_exp(table t, A_exp exp) {
    switch(exp->kind) {
        case A_idExp:
            return table_find(t, exp->u.id)->val;
            break;
        case A_numExp:
            return exp->u.num;
            break;
        case A_opExp:
            switch(exp->u.op.oper) {
                case A_plus:
                    return interp_exp(t, exp->u.op.left) + interp_exp(t, exp->u.op.right);
                case A_minus:
                    return interp_exp(t, exp->u.op.left) - interp_exp(t, exp->u.op.right);
                case A_times:
                    return interp_exp(t, exp->u.op.left) * interp_exp(t, exp->u.op.right);
                case A_div:
                    return interp_exp(t, exp->u.op.left) / interp_exp(t, exp->u.op.right);
            }
            break;
        case A_eseqExp:
            interp_stm(t, exp->u.eseq.stm);
            return interp_exp(t, exp->u.eseq.exp);
            break;
    }
}

int interp_print(table t, A_expList exps) {
    switch(exps->kind) {
        case A_pairExpList:
            printf("%d ", interp_exp(t, exps->u.pair.head));
            interp_print(t, exps->u.pair.tail);
            break;
        case A_lastExpList:
            printf("%d\n", interp_exp(t, exps->u.last));
            break;
    }
}























