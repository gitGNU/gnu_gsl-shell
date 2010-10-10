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

val main :
  (Lexing.lexbuf  -> token) -> Lexing.lexbuf -> Proto.system_def list
