%{
open Math
open Proto
%}

%token <string> NUMBER
%token <string> IDENT
%token PLUS MINUS TIMES DIV POW
%token LPAREN RPAREN SQLPAREN SQRPAREN
%token COMMA EQUAL SYSTEM END EOF
%left PLUS MINUS
%left TIMES DIV
%nonassoc UMINUS
%nonassoc POW
%start main
%type <Proto.system_def list> main
%%

expr:
   IDENT                      { Var(Var.get_var $1) }
 | NUMBER                     { Const(Num.num_of_string $1) }
 | LPAREN expr RPAREN         { $2 }
 | expr PLUS expr             { build_sum $1 $3 }
 | expr MINUS expr            { build_sum $1 (minus_expr $3) }
 | expr TIMES expr            { build_product $1 $3 }
 | expr DIV expr              { Fraction($1, $3) }
 | expr POW expr              { build_power $1 $3 }
 | IDENT LPAREN exprls RPAREN { Call($1, $3) }
 | MINUS expr %prec UMINUS    { minus_expr $2 }
;

exprls:
   expr                        { $1 :: [] }
 | exprls COMMA expr           { $3 :: $1 }
;

csids: /* */          { [] }
  | IDENT             { $1 :: [] }
  | csids COMMA IDENT { $3 :: $1 }
;

proto: IDENT LPAREN csids RPAREN { ($1, $3) }
;

eqn:
   IDENT EQUAL expr    { $1, $3 }
;

normeqns: 
    eqn                 { $1 :: [] }
  | normeqns COMMA eqn  { $3 :: $1 }
;

system:
   SYSTEM SQLPAREN csids SQRPAREN proto normeqns END { {iparam= $3; proto= $5; equations= $6} }
;
   

defs:
    system                { $1 :: [] }
  | defs system           { $2 :: $1 }
;

main: defs EOF            { $1 }
;
