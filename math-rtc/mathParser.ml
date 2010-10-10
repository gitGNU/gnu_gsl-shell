type token =
  | NUMBER of (string)
  | IDENT of (string)
  | PLUS
  | MINUS
  | TIMES
  | DIV
  | POW
  | LPAREN
  | RPAREN
  | SQLPAREN
  | SQRPAREN
  | COMMA
  | EQUAL
  | SYSTEM
  | END
  | EOF

open Parsing;;
# 2 "mathParser.mly"
open Math
open Variable

let globals = Variable.create_symtab ()
let symtab = ref globals

# 28 "mathParser.ml"
let yytransl_const = [|
  259 (* PLUS *);
  260 (* MINUS *);
  261 (* TIMES *);
  262 (* DIV *);
  263 (* POW *);
  264 (* LPAREN *);
  265 (* RPAREN *);
  266 (* SQLPAREN *);
  267 (* SQRPAREN *);
  268 (* COMMA *);
  269 (* EQUAL *);
  270 (* SYSTEM *);
  271 (* END *);
    0 (* EOF *);
    0|]

let yytransl_block = [|
  257 (* NUMBER *);
  258 (* IDENT *);
    0|]

let yylhs = "\255\255\
\002\000\002\000\002\000\002\000\002\000\002\000\002\000\002\000\
\002\000\002\000\003\000\003\000\004\000\004\000\004\000\005\000\
\006\000\007\000\007\000\008\000\009\000\009\000\001\000\000\000"

let yylen = "\002\000\
\001\000\001\000\003\000\003\000\003\000\003\000\003\000\003\000\
\004\000\002\000\001\000\003\000\000\000\001\000\003\000\004\000\
\003\000\001\000\003\000\007\000\001\000\002\000\002\000\002\000"

let yydefred = "\000\000\
\000\000\000\000\000\000\024\000\021\000\000\000\000\000\023\000\
\022\000\014\000\000\000\000\000\000\000\000\000\000\000\015\000\
\000\000\000\000\018\000\000\000\000\000\000\000\000\000\020\000\
\016\000\002\000\000\000\000\000\000\000\000\000\019\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\003\000\000\000\000\000\000\000\000\000\008\000\009\000\
\000\000\000\000"

let yydgoto = "\002\000\
\004\000\030\000\041\000\011\000\015\000\019\000\020\000\005\000\
\006\000"

let yysindex = "\255\255\
\000\255\000\000\010\255\000\000\000\000\001\000\024\255\000\000\
\000\000\000\000\040\255\059\255\069\255\028\255\071\255\000\000\
\024\255\073\255\000\000\248\254\252\254\031\255\071\255\000\000\
\000\000\000\000\030\255\031\255\031\255\085\255\000\000\031\255\
\089\255\078\255\031\255\031\255\031\255\031\255\031\255\085\255\
\013\255\000\000\088\255\088\255\089\255\089\255\000\000\000\000\
\031\255\085\255"

let yyrindex = "\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\067\255\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\036\255\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\012\255\000\000\000\000\034\255\000\000\000\000\
\025\255\000\000\000\000\000\000\000\000\000\000\000\000\053\255\
\000\000\000\000\055\255\065\255\038\255\051\255\000\000\000\000\
\000\000\063\255"

let yygindex = "\000\000\
\000\000\230\255\000\000\059\000\000\000\074\000\000\000\092\000\
\000\000"

let yytablesize = 271
let yytable = "\001\000\
\008\000\033\000\034\000\023\000\025\000\040\000\024\000\013\000\
\043\000\044\000\045\000\046\000\047\000\003\000\001\000\001\000\
\001\000\001\000\001\000\007\000\001\000\048\000\050\000\001\000\
\049\000\010\000\001\000\010\000\010\000\010\000\010\000\026\000\
\027\000\010\000\028\000\017\000\010\000\032\000\029\000\010\000\
\006\000\006\000\006\000\006\000\013\000\017\000\006\000\013\000\
\017\000\006\000\012\000\013\000\006\000\007\000\007\000\007\000\
\007\000\004\000\004\000\007\000\014\000\011\000\007\000\004\000\
\011\000\007\000\004\000\005\000\005\000\004\000\016\000\012\000\
\018\000\005\000\012\000\021\000\005\000\013\000\013\000\005\000\
\035\000\036\000\037\000\038\000\039\000\022\000\042\000\035\000\
\036\000\037\000\038\000\039\000\037\000\038\000\039\000\039\000\
\031\000\009\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\003\000"

let yycheck = "\001\000\
\000\000\028\000\029\000\012\001\009\001\032\000\015\001\012\001\
\035\000\036\000\037\000\038\000\039\000\014\001\003\001\004\001\
\005\001\006\001\007\001\010\001\009\001\009\001\049\000\012\001\
\012\001\002\001\015\001\003\001\004\001\005\001\006\001\001\001\
\002\001\009\001\004\001\008\001\012\001\008\001\008\001\015\001\
\003\001\004\001\005\001\006\001\009\001\012\001\009\001\012\001\
\015\001\012\001\011\001\012\001\015\001\003\001\004\001\005\001\
\006\001\003\001\004\001\009\001\002\001\009\001\012\001\009\001\
\012\001\015\001\012\001\003\001\004\001\015\001\002\001\009\001\
\002\001\009\001\012\001\017\000\012\001\011\001\012\001\015\001\
\003\001\004\001\005\001\006\001\007\001\013\001\009\001\003\001\
\004\001\005\001\006\001\007\001\005\001\006\001\007\001\007\001\
\023\000\006\000\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\014\001"

let yynames_const = "\
  PLUS\000\
  MINUS\000\
  TIMES\000\
  DIV\000\
  POW\000\
  LPAREN\000\
  RPAREN\000\
  SQLPAREN\000\
  SQRPAREN\000\
  COMMA\000\
  EQUAL\000\
  SYSTEM\000\
  END\000\
  EOF\000\
  "

let yynames_block = "\
  NUMBER\000\
  IDENT\000\
  "

let yyact = [|
  (fun _ -> failwith "parser")
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 0 : string) in
    Obj.repr(
# 24 "mathParser.mly"
                              ( Var(Variable.lookup_id !symtab _1) )
# 198 "mathParser.ml"
               : 'expr))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 0 : string) in
    Obj.repr(
# 25 "mathParser.mly"
                              ( Const(Num.num_of_string _1) )
# 205 "mathParser.ml"
               : 'expr))
; (fun __caml_parser_env ->
    let _2 = (Parsing.peek_val __caml_parser_env 1 : 'expr) in
    Obj.repr(
# 26 "mathParser.mly"
                              ( _2 )
# 212 "mathParser.ml"
               : 'expr))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : 'expr) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : 'expr) in
    Obj.repr(
# 27 "mathParser.mly"
                              ( build_sum _1 _3 )
# 220 "mathParser.ml"
               : 'expr))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : 'expr) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : 'expr) in
    Obj.repr(
# 28 "mathParser.mly"
                              ( build_sum _1 (minus_expr _3) )
# 228 "mathParser.ml"
               : 'expr))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : 'expr) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : 'expr) in
    Obj.repr(
# 29 "mathParser.mly"
                              ( build_product _1 _3 )
# 236 "mathParser.ml"
               : 'expr))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : 'expr) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : 'expr) in
    Obj.repr(
# 30 "mathParser.mly"
                              ( Fraction(_1, _3) )
# 244 "mathParser.ml"
               : 'expr))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : 'expr) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : 'expr) in
    Obj.repr(
# 31 "mathParser.mly"
                              ( build_power _1 _3 )
# 252 "mathParser.ml"
               : 'expr))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 3 : string) in
    let _3 = (Parsing.peek_val __caml_parser_env 1 : 'exprls) in
    Obj.repr(
# 32 "mathParser.mly"
                              ( Call(_1, _3) )
# 260 "mathParser.ml"
               : 'expr))
; (fun __caml_parser_env ->
    let _2 = (Parsing.peek_val __caml_parser_env 0 : 'expr) in
    Obj.repr(
# 33 "mathParser.mly"
                              ( minus_expr _2 )
# 267 "mathParser.ml"
               : 'expr))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 0 : 'expr) in
    Obj.repr(
# 37 "mathParser.mly"
                               ( _1 :: [] )
# 274 "mathParser.ml"
               : 'exprls))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : 'exprls) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : 'expr) in
    Obj.repr(
# 38 "mathParser.mly"
                               ( _3 :: _1 )
# 282 "mathParser.ml"
               : 'exprls))
; (fun __caml_parser_env ->
    Obj.repr(
# 41 "mathParser.mly"
                      ( [] )
# 288 "mathParser.ml"
               : 'csids))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 0 : string) in
    Obj.repr(
# 42 "mathParser.mly"
                      ( _1 :: [] )
# 295 "mathParser.ml"
               : 'csids))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : 'csids) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : string) in
    Obj.repr(
# 43 "mathParser.mly"
                      ( _3 :: _1 )
# 303 "mathParser.ml"
               : 'csids))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 3 : string) in
    let _3 = (Parsing.peek_val __caml_parser_env 1 : 'csids) in
    Obj.repr(
# 46 "mathParser.mly"
                                 ( (_1, _3) )
# 311 "mathParser.ml"
               : 'proto))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : string) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : 'expr) in
    Obj.repr(
# 50 "mathParser.mly"
                       ( _1, _3 )
# 319 "mathParser.ml"
               : 'eqn))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 0 : 'eqn) in
    Obj.repr(
# 54 "mathParser.mly"
                        ( _1 :: [] )
# 326 "mathParser.ml"
               : 'normeqns))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : 'normeqns) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : 'eqn) in
    Obj.repr(
# 55 "mathParser.mly"
                        ( _3 :: _1 )
# 334 "mathParser.ml"
               : 'normeqns))
; (fun __caml_parser_env ->
    let _3 = (Parsing.peek_val __caml_parser_env 4 : 'csids) in
    let _5 = (Parsing.peek_val __caml_parser_env 2 : 'proto) in
    let _6 = (Parsing.peek_val __caml_parser_env 1 : 'normeqns) in
    Obj.repr(
# 59 "mathParser.mly"
                                                     ( 
     print_string "BINDING\n";
     let name, args = _5 in
     let st = !symtab in
     let bindings = Variable.function_symtab_bind st.map _3 args in
       symtab := Variable.create_symtab ();
       {Proto.name= name; 
	Proto.symtab= st; 
	Proto.bindings= bindings; 
	Proto.equations= _6}
   )
# 353 "mathParser.ml"
               : 'system))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 0 : 'system) in
    Obj.repr(
# 73 "mathParser.mly"
                          ( _1 :: [] )
# 360 "mathParser.ml"
               : 'defs))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 1 : 'defs) in
    let _2 = (Parsing.peek_val __caml_parser_env 0 : 'system) in
    Obj.repr(
# 74 "mathParser.mly"
                          ( _2 :: _1 )
# 368 "mathParser.ml"
               : 'defs))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 1 : 'defs) in
    Obj.repr(
# 77 "mathParser.mly"
                          ( _1 )
# 375 "mathParser.ml"
               : Proto.system_def list))
(* Entry main *)
; (fun __caml_parser_env -> raise (Parsing.YYexit (Parsing.peek_val __caml_parser_env 0)))
|]
let yytables =
  { Parsing.actions=yyact;
    Parsing.transl_const=yytransl_const;
    Parsing.transl_block=yytransl_block;
    Parsing.lhs=yylhs;
    Parsing.len=yylen;
    Parsing.defred=yydefred;
    Parsing.dgoto=yydgoto;
    Parsing.sindex=yysindex;
    Parsing.rindex=yyrindex;
    Parsing.gindex=yygindex;
    Parsing.tablesize=yytablesize;
    Parsing.table=yytable;
    Parsing.check=yycheck;
    Parsing.error_function=parse_error;
    Parsing.names_const=yynames_const;
    Parsing.names_block=yynames_block }
let main (lexfun : Lexing.lexbuf -> token) (lexbuf : Lexing.lexbuf) =
   (Parsing.yyparse yytables 1 lexfun lexbuf : Proto.system_def list)
