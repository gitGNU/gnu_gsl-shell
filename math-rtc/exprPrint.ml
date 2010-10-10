open Math
open Printf


let mprint_list buffer print_func simbolo lista =
  let f () x =
    Buffer.add_string buffer simbolo;
    print_func buffer x in
  match lista with
    [] -> ()
  | [x] -> print_func buffer x
  | h::t ->
      print_func buffer h;
      List.fold_left f () t


let with_brackets buffer condition x =
  if condition then Buffer.add_char buffer '(';
  Buffer.add_buffer buffer x;
  if condition then Buffer.add_char buffer ')'


let debug_on = false

let print_list printfun printsep ls =
  let f first e = 
    if not first then printsep ();
    printfun e; false in
  let _ = List.fold_left f true ls in ()

let rec print_mon ctx signed buffer (f, lett) =
  let fs buf (b, e) =
    if e = 1 then
      print_expr ctx buffer 20 b
    else
      begin
	print_expr ctx buffer 100 b;
	bprintf buffer "^%i" e
      end in
  let is_neg = f < zero in
  let sym, f' =
    if is_neg then (" - ", Num.minus_num f) else
    if signed then (" + ", f) else ("", f) in
  if debug_on then Buffer.add_char buffer '[';
  bprintf buffer "%s" sym;
  if f' <> unity then bprintf buffer "%s " (Num.string_of_num f');
  mprint_list buffer fs " " lett;
  if debug_on then Buffer.add_char buffer ']'
and print_poly ctx buffer (noto, term) =
  begin
    match term with
      x :: tail ->
	print_mon ctx false buffer x;
	mprint_list buffer (print_mon ctx true) "" tail
    | [] -> ()
  end;
  if noto < zero then
    let n' = c_minus noto in
    bprintf buffer " - %s" (Num.string_of_num n')
  else
    if noto > zero then bprintf buffer " + %s" (Num.string_of_num noto)
and print_binary ctx buffer prio simbolo (a, b) =
  print_expr ctx buffer prio a;
  Buffer.add_string buffer simbolo;
  print_expr ctx buffer prio b
and print_call ctx buf f args =
  Buffer.add_string buf f;
  Buffer.add_char buf '(';
  print_list (fun ex -> print_expr ctx buf 0 ex) (fun () -> Buffer.add_string buf ", ") args;
  Buffer.add_char buf ')'
and print_expr ctx buffer prio e =
  let parziale = Buffer.create 64 in
  let this_prio =
    match e with
      Const c -> bprintf buffer "%s" (Num.string_of_num c); 1000
    | Var n -> bprintf buffer "%s" (Variable.lookup_name ctx n); 1000
    | Power (a, b) -> print_binary ctx parziale 100 "^" (a, b); 100
    | Sum s -> print_poly ctx parziale s; 10
    | Product p -> print_mon ctx false parziale p; 20
    | Fraction (a, b) -> print_binary ctx parziale 30 "/" (a, b); 30
    | Call (f, args) -> print_call ctx parziale f args; 200
  in
  with_brackets buffer (prio >= this_prio) parziale

let print ctx ex =
  let buf = Buffer.create 128 in
  print_expr ctx buf 0 ex;
  Buffer.contents buf
