
type variable = int

(* -------- Variable's table -------- *)

let next_var_id = ref 0

let var_table = ref []

let get_var (name : string) =
  try
    List.assoc name !var_table
  with Not_found ->
    let id = !next_var_id in
    var_table := (name, id) :: !var_table;
    incr next_var_id;
    id

let retrieve_name number =
  let funzione (_, x) = (x = number) in
  let (name, _) = List.find funzione !var_table in
  name
