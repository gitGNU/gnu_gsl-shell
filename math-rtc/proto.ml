open Math

type system_def = {
  name: string;
  symtab: Variable.symbol_table;
  bindings: (int * Variable.t) list;
  equations: (string * expr) list
}

let print_system sys =
  let eqns = sys.equations in
  let eqnfun (id, ex) = id ^ " = " ^ (ExprPrint.print sys.symtab ex) in
  List.map eqnfun eqns
