type system_def = {
  name : string;
  symtab : Variable.symbol_table;
  bindings : (int * Variable.t) list;
  equations : (string * Math.expr) list;
}
val print_system : system_def -> string list
