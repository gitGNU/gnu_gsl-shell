open Proto
open Parser
open ExprPrint

let print_system sys =
  let eqns = sys.equations in
  let eqnfun (id, ex) = id ^ " = " ^ (ExprPrint.print ex) in
  List.map eqnfun eqns
