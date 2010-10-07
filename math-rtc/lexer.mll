{
  open Parser        (* The type token is defined in parser.mli *)
  exception Eof

  let keyword_table = Hashtbl.create 16
  let _ = List.iter (fun (kwd, tok) -> Hashtbl.add keyword_table kwd tok)
              [ "system",   SYSTEM; "end", END ]

  let treat_id id =
    try
      Hashtbl.find keyword_table id
    with Not_found ->
      IDENT(id)
}

rule token = parse
   [' ' '\t' '\n']     { token lexbuf }     (* skip blanks *)
 | ','            { COMMA }
 | ['0'-'9'] +  * { NUMBER(Lexing.lexeme lexbuf) }
 | ['A'-'Z' 'a'-'z'] ['A'-'Z' 'a'-'z' '0'-'9' '_'] * '\'' * { treat_id (Lexing.lexeme lexbuf) }
 | '+'            { PLUS }
 | '='            { EQUAL }
 | '-'            { MINUS }
 | '*'            { TIMES }
 | '/'            { DIV }
 | '^'            { POW }
 | '('            { LPAREN }
 | ')'            { RPAREN }
 | '['            { SQLPAREN }
 | ']'            { SQRPAREN }
 | eof            { EOF }
