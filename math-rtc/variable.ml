
exception Undefined_variable of string

type t =
    Global of string
  | Argument of int * string
  | Parameter of int * string

type symbol_table = {
  mutable id: int; 
  map: (string, int) Hashtbl.t
}

let create_symtab () = 
  let h : (string, int) Hashtbl.t = Hashtbl.create 16 in
    {id= 0; map= h}

let get_new_id symtab =
  let n = symtab.id in
    symtab.id <- symtab.id + 1;
    n

let lookup_id symtab str =
  try
    let id = Hashtbl.find symtab.map str in id
  with Not_found ->
    let id = get_new_id symtab in
      Hashtbl.add symtab.map str id;
      print_string "VARIABLE: "; print_int id; print_newline ();
      id

let lookup_name symtab id =
  let search s id' found =
    if id' = id then Some s else found in
  let name' = Hashtbl.fold search symtab.map None in
    match name' with
	Some n -> n
      | None -> raise Not_found

let rec memi a ls n =
  match ls with
      [] -> None
    | h :: tl -> if h = a then Some n else memi a tl (n-1)

(*
let string_of_var v =
  match v with
      Global s -> Printf.sprintf "Global(%s)" s
    | Parameter (n,s) -> Printf.sprintf "Param#%i(%s)" n s
    | Argument (n,s) -> Printf.sprintf "Arg#%i(%s)" n s
*)

let function_symtab_bind symtab params args =
  let bind str id bindings =
    let var = 
      let n' = memi str params (List.length params) in
	match n' with
	    None -> 
	      let k' = memi str args (List.length args) in
		begin
		  match k' with
		      None -> raise (Undefined_variable str)
		    | Some k -> Argument(k, str)
		end
	  | Some n -> Parameter(n, str)
    in
      (id, var) :: bindings 
  in
    Hashtbl.fold bind symtab []
