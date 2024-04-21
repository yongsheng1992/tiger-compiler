#include <stdio.h>
#include "slp.h"
#include "util.h"
#include "prog1.h"

typedef struct table *Table_;

int maxargsExp(A_exp);
int maxargsExpList(A_expList);

struct table
{
    string id;
    int value;
    Table_ tail;
};

Table_ Table(string id, int value, struct table *tail) {
    Table_ t = checked_malloc(sizeof(*t));
    t->id = id;
    t->value = value;
    t->tail = tail;
    return t;
}

Table_ interpStm(A_stm s, Table_ t);

Table_ update(Table_ t, string key, int value) {
    Table_ t1 = Table(key, value, t);
    return t1;
}

int lookup(Table_ t, string key) {
    Table_ temp;
    for (temp = t; temp->tail != NULL && temp->id != key; temp = temp->tail) {
    }
    return temp->value;
}

struct intAndTable {
    int i;
    Table_ t;
};

typedef struct intAndTable *IntAndTable_;
IntAndTable_ interpExp(A_exp e, Table_ t);

IntAndTable_ IntAndTable(int i, Table_ t) {
    IntAndTable_ intAndTable = checked_malloc(sizeof(*intAndTable));
    intAndTable->i = i;
    intAndTable->t = t;
    return intAndTable;
}

IntAndTable_ interpExp(A_exp e, Table_ t) {
    int i;
    IntAndTable_ intAndTable1;
    IntAndTable_ intAndTable2;
    Table_ t1;
    switch (e->kind) {
    case A_idExp:
        i = lookup(t, e->u.id);
        intAndTable1 = IntAndTable(i, t);
        return intAndTable1;
    case A_numExp:
        i = e->u.num;
        intAndTable1 = IntAndTable(i, t);
        return intAndTable1;
    case A_opExp:
        intAndTable1 = interpExp(e->u.op.left, t);
        intAndTable2 = interpExp(e->u.op.right, t);
        if (e->u.op.oper == A_minus) {
            i = intAndTable1->i - intAndTable2->i;
        } else if (e->u.op.oper == A_plus) {
            i = intAndTable1->i + intAndTable2->i;
        } else if (e->u.op.oper == A_times) {
            i = intAndTable1->i * intAndTable2->i;
        } else {
            i = intAndTable1->i / intAndTable2->i;
        }
        return IntAndTable(i, t);
    case A_eseqExp:
        t1 = interpStm(e->u.eseq.stm, t);
        intAndTable1 = interpExp(e->u.eseq.exp, t1);
        return intAndTable1;
    default:
        break;
    }
    return NULL;
}

Table_ interpStm(A_stm s, Table_ t) {
    Table_ t1;
    IntAndTable_ intAndTable;
    A_expList exps;
    switch (s->kind) {
    case A_compoundStm:
        t1 = interpStm(s->u.compound.stm1, t);
        t1 = interpStm(s->u.compound.stm2, t1);
        return t1;
    case A_assignStm:
        intAndTable = interpExp(s->u.assign.exp, t);
        t1 = update(t, s->u.assign.id, intAndTable->i);
        return t1;
    case A_printStm:
        if (s->u.print.exps->kind == A_pairExpList) {
            for (exps = s->u.print.exps; exps->kind == A_pairExpList;exps = exps->u.pair.tail) {
                intAndTable = interpExp(exps->u.pair.head, t);
                printf("%d ", intAndTable->i);
            }
            intAndTable = interpExp(exps->u.last, intAndTable->t);
            printf("%d ", intAndTable->i);
        } else {
            intAndTable = interpExp(s->u.print.exps->u.last, t);
            printf("%d ", intAndTable->i);
        }
        printf("\n", intAndTable->i);
        return intAndTable->t;
    default:
        break;
    }
    return t;
}




int max(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

// maxargs calculate the max args print exp in A_stm
int maxargs(A_stm stm) {
    A_expList exps;
    switch (stm->kind) {
    case A_compoundStm:
        return max(maxargs(stm->u.compound.stm1), maxargs(stm->u.compound.stm2));
    case A_assignStm:
        return maxargsExp(stm->u.assign.exp);
    case A_printStm:
        exps = stm->u.print.exps;
        if (exps->kind == A_pairExpList) {
            int ans = max(
                maxargsExp(exps->u.pair.head),
                maxargsExpList(exps->u.pair.tail)
            );
            return max(2, ans);
        }

        int ans = maxargsExp(exps->u.last);
        return max(1, ans);
    default:
        break;
    }
    return 0;
}


int maxargsExp(A_exp exp) {
    switch (exp->kind){
    case A_opExp:
        return max(
            maxargsExp(exp->u.op.left),
            maxargsExp(exp->u.op.right)
        );
    case A_eseqExp:
        return max(
            maxargs(exp->u.eseq.stm),
            maxargsExp(exp->u.eseq.exp)
        );
    default:
        break;
    }
    return 0;
}

int maxargsExpList(A_expList expList) {
    if (expList->kind == A_pairExpList) {
        return max(
            maxargsExp(expList->u.pair.head),
            maxargsExpList(expList->u.pair.tail)
        );
    }
    return maxargsExp(expList->u.last);
}


void interp(A_stm stm) {
    Table_ t = interpStm(stm, NULL);
}

int main() {
    A_stm stm = prog();
    printf("maxargs: %d\n", maxargs(stm));
    interp(stm);
    return 0;
}

