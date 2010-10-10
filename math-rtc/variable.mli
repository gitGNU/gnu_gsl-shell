exception Undefined_variable of string
type t =
    Global of string
  | Argument of int * string
  | Parameter of int * string

type symbol_table = {
  mutable id: int; 
  map: (string, int) Hashtbl.t
}

val create_symtab : unit -> symbol_table
val lookup_id : symbol_table -> string -> int
val lookup_name : symbol_table -> int -> string
val function_symtab_bind :
  (string, int) Hashtbl.t -> string list -> string list -> (int * t) list
