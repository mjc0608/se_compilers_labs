%{
#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "symbol.h" 
#include "errormsg.h"
#include "absyn.h"

int yylex(void); /* function prototype */

A_exp absyn_root;

void yyerror(char *s)
{
 EM_error(EM_tokPos, "%s", s);
 exit(1);
}

// #define DEBUG

#ifdef DEBUG
#define dbg_print(left, right) printf("%s <- %s\n", (left), (right))
#else 
#define dbg_print(left, right)
#endif

%}


%union {
	int pos;
	int ival;
	string sval;
    S_symbol symbol;
	A_var var;
	A_exp exp;
	A_dec dec;
    A_ty ty;
    A_decList declist;
    A_expList explist;
    A_field field;
    A_fieldList fields;
    A_fundec fundec;
    A_fundecList fundeclist;
    A_namety namety;
    A_nametyList nametylist;
    A_efield efield;
    A_efieldList efields;
	}

%token <sval> ID STRING
%token <ival> INT

%left AND OR
%nonassoc EQ LE LT GE GT NEQ
%left PLUS MINUS
%left TIMES DIVIDE
%left UMINUS
%nonassoc LBRACE
%nonassoc OF


%token 
  COMMA COLON SEMICOLON LPAREN RPAREN LBRACK RBRACK 
  LBRACE RBRACE DOT 
  PLUS MINUS TIMES DIVIDE EQ NEQ LT LE GT GE
  AND OR ASSIGN
  ARRAY IF THEN ELSE WHILE FOR TO DO LET IN END OF 
  BREAK NIL
  FUNCTION VAR TYPE 

%type <exp> exp program op_exp if_exp array_exp record_exp
%type <var> var 
%type <dec> dec vardec
%type <fundec> fundec
%type <fundeclist> fundeclist
%type <namety> typedec
%type <nametylist> typedeclist
%type <explist> explist explist_nonempty explist_funcall explist_funcall_nonempty
%type <declist> declist declist_nonempty
%type <field> tyfield
%type <fields> tyfields tyfields_nonempty
%type <efield> record_create_field
%type <efields> record_create_fields record_create_fields_nonempty
%type <ty> ty

%start program

%%

program:   exp    {absyn_root=$1;}
;

exp:     var                                {dbg_print("exp", "var"); $$=A_VarExp(EM_tokPos, $1);}
        |NIL                                {dbg_print("exp", "NIL"); $$=A_NilExp(EM_tokPos);}
        |INT                                {dbg_print("exp", "INT"); $$=A_IntExp(EM_tokPos, $1);}
        |STRING                             {dbg_print("exp", "STRING"); $$=A_StringExp(EM_tokPos, $1);}
        |ID LPAREN explist_funcall RPAREN   {dbg_print("exp", "ID LPAREN explist RPAREN"); $$=A_CallExp(EM_tokPos, S_Symbol($1), $3);}
        |LPAREN explist RPAREN              {dbg_print("exp", "LPAREN explist RPAREN"); $$=A_SeqExp(EM_tokPos, $2);}
        |op_exp                             {dbg_print("exp", "op_exp"); $$=$1;}
        |var ASSIGN exp                     {dbg_print("exp", "var ASSIGN exp"); $$=A_AssignExp(EM_tokPos, $1, $3);}
        |if_exp                             {dbg_print("exp", "if_exp"); $$=$1;}
        |WHILE exp DO exp                   {dbg_print("exp", "WHILE exp DO exp"); $$=A_WhileExp(EM_tokPos, $2, $4);}
        |FOR ID ASSIGN exp TO exp DO exp    {dbg_print("exp", "FOR ID ASSIGN exp TO exp DO exp"); $$=A_ForExp(EM_tokPos, S_Symbol($2), $4, $6, $8);}
        |BREAK                              {dbg_print("exp", "BREAK"); $$=A_BreakExp(EM_tokPos);}
        |LET declist IN explist END         {dbg_print("exp", "LET declist IN explist END"); $$=A_LetExp(EM_tokPos, $2, A_SeqExp(EM_tokPos, $4));}
        |array_exp                          {dbg_print("exp", "array_exp"); $$=$1;}
        |record_exp                         {dbg_print("exp", "record_exp"); $$=$1;}
        |MINUS exp %prec UMINUS             {dbg_print("exp", "MINUS exp %prec UMINUS"); $$=A_OpExp(EM_tokPos, A_minusOp, A_IntExp(EM_tokPos, 0), $2);}
;

explist:     /* empty */                    {dbg_print("explist", "empty"); $$=NULL;}
            |explist_nonempty               {dbg_print("explist", "explist_nonempty"); $$=$1;}
;

explist_nonempty:    exp                    {dbg_print("explist_nonempty", "exp"); $$=A_ExpList($1, NULL);}
                    |exp SEMICOLON explist_nonempty     {dbg_print("explist_nonempty", "exp SEMICOLON explist_nonempty"); $$=A_ExpList($1, $3);}
;

explist_funcall:     /* empty */            {dbg_print("explist_funcall", "empty"); $$=NULL;}
                    |explist_funcall_nonempty   {dbg_print("explist_funcall", "explist_funcall_nonempty"); $$=$1;}
;

explist_funcall_nonempty:    exp            {dbg_print("explist_funcall_nonempty", "exp"); $$=A_ExpList($1, NULL);}
                            |exp COMMA explist_funcall_nonempty     {dbg_print("explist_funcall_nonempty", "exp COMMA explist_funcall_nonempty"); $$=A_ExpList($1, $3);}
;

ty:  ID                                     {dbg_print("ty", "ID"); $$=A_NameTy(EM_tokPos, S_Symbol($1));}
    |LBRACE tyfields RBRACE                 {dbg_print("ty", "LBRACE tyfields RBRACE"); $$=A_RecordTy(EM_tokPos, $2);}
    |ARRAY OF ID                            {dbg_print("ty", "ARRAY OF ID"); $$=A_ArrayTy(EM_tokPos, S_Symbol($3));}
;

tyfield:     ID COLON ID                    {dbg_print("tyfield", "ID COLON ID"); $$=A_Field(EM_tokPos, S_Symbol($1), S_Symbol($3));}
;

tyfields:    /* empty */                    {dbg_print("tyfields", "empty"); $$=NULL;}
            |tyfields_nonempty              {dbg_print("tyfields", "tyfields_nonempty"); $$=$1;}
;

tyfields_nonempty:   tyfield                {dbg_print("tyfields_nonempty", "tyfield"); $$=A_FieldList($1, NULL);}
                    |tyfield COMMA tyfields_nonempty      {dbg_print("tyfields_nonempty", "tyfield COMMA tyfields_nonempty"); $$=A_FieldList($1, $3);}
;

dec:     typedeclist                        {dbg_print("dec", "typedeclist"); $$=A_TypeDec(EM_tokPos, $1);}
        |vardec                             {dbg_print("dec", "vardec"); $$=$1;}
        |fundeclist                         {dbg_print("dec", "fundec"); $$=A_FunctionDec(EM_tokPos, $1);}
;

declist:     /* empty */                    {dbg_print("declist", "empty"); $$=NULL;}
            |declist_nonempty               {dbg_print("declist", "declist_nonempty"); $$=$1;}
;

declist_nonempty:    dec                    {dbg_print("declist_nonempty", "dec"); $$=A_DecList($1, NULL);}
                    |dec declist_nonempty   {dbg_print("declist_nonempty", "dec declist_nonempty"); $$=A_DecList($1, $2);}
;

vardec:  VAR ID ASSIGN exp                  {dbg_print("vardec", "VAR ID ASSIGN exp"); $$=A_VarDec(EM_tokPos, S_Symbol($2), NULL, $4);}
        |VAR ID COLON ID ASSIGN exp         {dbg_print("vardec", "VAR ID COLON ID ASSIGN exp"); $$=A_VarDec(EM_tokPos, S_Symbol($2), S_Symbol($4), $6);}
;

typedec:    TYPE ID EQ ty                   {dbg_print("typedec", "TYPE ID EQ ty"); $$=A_Namety(S_Symbol($2), $4);}
;

typedeclist:     typedec                    {dbg_print("typedeclist", "typedec"); $$=A_NametyList($1, NULL);}
                |typedec typedeclist        {dbg_print("typedeclist", "typedec typedeclist"); $$=A_NametyList($1, $2);}
;

fundec:  FUNCTION ID LPAREN tyfields RPAREN EQ exp      {dbg_print("fundec", "FUNCTION ID LPAREN tyfields RPAREN EQ exp"); $$=A_Fundec(EM_tokPos, S_Symbol($2), $4, NULL, $7);}
        |FUNCTION ID LPAREN tyfields RPAREN COLON ID EQ exp     {dbg_print("fundec", "FUNCTION ID LPAREN tyfields RPAREN COLON ID EQ exp"); $$=A_Fundec(EM_tokPos, S_Symbol($2), $4, S_Symbol($7), $9);}
;

fundeclist:      fundec                     {dbg_print("fundeclist", "fundec"); $$=A_FundecList($1, NULL);}
                |fundec fundeclist          {dbg_print("fundeclist", "fundec fundeclist"); $$=A_FundecList($1, $2);}
;

var:     ID                                 {dbg_print("var", "ID"); $$=A_SimpleVar(EM_tokPos, S_Symbol($1));}
        |var DOT ID                         {dbg_print("var", "var DOT ID"); $$=A_FieldVar(EM_tokPos, $1, S_Symbol($3));}
        |ID LBRACK exp RBRACK               {dbg_print("var", "ID LBRACK exp RBRACK"); $$=A_SubscriptVar(EM_tokPos, A_SimpleVar(EM_tokPos, S_Symbol($1)), $3);}
        |var LBRACK exp RBRACK              {dbg_print("var", "var LBRACK exp RBRACK"); $$=A_SubscriptVar(EM_tokPos, $1, $3);}
;

op_exp:      exp PLUS exp                   {dbg_print("op_exp", "exp PLUS exp"); $$=A_OpExp(EM_tokPos, A_plusOp, $1, $3);}
            |exp MINUS exp                  {dbg_print("op_exp", "exp PLUS exp"); $$=A_OpExp(EM_tokPos, A_minusOp, $1, $3);}
            |exp TIMES exp                  {dbg_print("op_exp", "exp PLUS exp"); $$=A_OpExp(EM_tokPos, A_timesOp, $1, $3);}
            |exp DIVIDE exp                 {dbg_print("op_exp", "exp PLUS exp"); $$=A_OpExp(EM_tokPos, A_divideOp, $1, $3);}
            |exp EQ exp                     {dbg_print("op_exp", "exp PLUS exp"); $$=A_OpExp(EM_tokPos, A_eqOp, $1, $3);}
            |exp NEQ exp                    {dbg_print("op_exp", "exp PLUS exp"); $$=A_OpExp(EM_tokPos, A_neqOp, $1, $3);}
            |exp LT exp                     {dbg_print("op_exp", "exp PLUS exp"); $$=A_OpExp(EM_tokPos, A_ltOp, $1, $3);}
            |exp LE exp                     {dbg_print("op_exp", "exp PLUS exp"); $$=A_OpExp(EM_tokPos, A_leOp, $1, $3);}
            |exp GT exp                     {dbg_print("op_exp", "exp PLUS exp"); $$=A_OpExp(EM_tokPos, A_gtOp, $1, $3);}
            |exp GE exp                     {dbg_print("op_exp", "exp PLUS exp"); $$=A_OpExp(EM_tokPos, A_geOp, $1, $3);}
            |exp AND exp                    {dbg_print("op_exp", "exp PLUS exp"); $$=A_IfExp(EM_tokPos, $1, $3, A_IntExp(EM_tokPos,0));}
            |exp OR exp                     {dbg_print("op_exp", "exp PLUS exp"); $$=A_IfExp(EM_tokPos, $1, A_IntExp(EM_tokPos, 1), $3);}
;

record_exp:  ID LBRACE record_create_fields RBRACE    {dbg_print("record_exp", "ID LBRACE record_create_fields RBRACE"); $$=A_RecordExp(EM_tokPos, S_Symbol($1), $3);}
; 

record_create_field:    ID EQ exp           {dbg_print("record_create_field", "ID EQ exp"); $$=A_Efield(S_Symbol($1), $3);}
;

record_create_fields:  /* empty */          {dbg_print("record_create_fields", "empty"); $$=NULL;}
                    |record_create_fields_nonempty    {dbg_print("record_create_fields", "record_create_fields_nonempty"); $$=$1;}
;

record_create_fields_nonempty:   record_create_field    {dbg_print("record_create_fields_nonempty", "record_create_field"); $$=A_EfieldList($1, NULL);}
                                |record_create_field COMMA record_create_fields_nonempty    {dbg_print("record_create_fields_nonempty", "record_create_field COMMA record_create_fields_nonempty"); $$=A_EfieldList($1, $3);}
;

array_exp:   ID LBRACK exp RBRACK OF exp    {dbg_print("array_exp", "ID LBRACE exp RBRACE OF exp"); $$=A_ArrayExp(EM_tokPos, S_Symbol($1), $3, $6);}
;

if_exp:  IF exp THEN exp ELSE exp           {dbg_print("if_exp", "IF exp THEN exp ELSE exp"); $$=A_IfExp(EM_tokPos, $2, $4, $6);}
        |IF exp THEN exp                    {dbg_print("if_exp", "IF exp THEN exp"); $$=A_IfExp(EM_tokPos, $2, $4, A_NilExp(EM_tokPos));}



	
