
let read str =
  let f = open_in str in
  let lexbuf = Lexing.from_channel f in
  let parse () = MathParser.main Lexer.token lexbuf in
    parse ()
