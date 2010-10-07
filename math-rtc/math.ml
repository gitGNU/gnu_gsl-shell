open Num
open Var

exception Indeterminate_expr
exception Division_by_zero

type function_ref = string

type 'a mon = 'a * (expr * int) list and
     'a poly = 'a * ('a mon) list and
     binary = expr * expr and
     function_call = function_ref * expr list and
     expr =
       Const of num
     | Var of variable
     | Power of binary
     | Fraction of binary
     | Sum of num poly
     | Product of num mon
     | Call of function_call

let unity = Int 1
let zero = Int 0

let c_minus = minus_num
let c_inv = function x -> unity // x 

let c_ipower x n = power_num x (num_of_int n)


(* --------- comparison functions ------------ *)

(* we cannot just use "compare" because big_int and ratio are both
   abstract types *)

let rec cmp_lists cmp_fun ls1 ls2 =
  match ls1, ls2 with
    ([], []) -> 0
  | ([], t::r) -> 1
  | (t::r, []) -> -1
  | (a::r, b::s) ->
      let cfr = cmp_fun a b in
      if cfr = 0 then cmp_lists cmp_fun r s else cfr

let cmp = compare

(* ---------- Assembling of expressions ---------- *)

let as_monomial e = (unity, [e, 1])

let rec add_to_lett lett (b, e) =
  if e = 0 then lett else
  match lett with
    [] -> [b, e]
  | (bx, ex)::tail ->
      let cfr = cmp bx b in
      if cfr < 0 then 
	(bx, ex) :: add_to_lett tail (b, e)
      else
	if cfr > 0 then (b, e) :: lett else
	if ex + e = 0 then tail else (b, e + ex) :: tail


let add_to_monomial m (b, e) =
  let (f, lett) = m in (f, add_to_lett lett (b, e))


let mult_mon m (f, ls) =
  let (fx, lx) = 
    List.fold_left add_to_monomial m ls in
  (fx */ f, lx)


let mon_reduce (f, ls) =
  match f, ls with
    (Int 0, _) -> Const zero
  | (_, []) -> Const f
  | (Int 1, [b, 1]) -> b
  | _ -> Product (f, ls)


let add_to_monomial_expr m e =
  let (fact, lett) = m in
  let nm =
    match e with
      Product (f, lst) ->
	let pm = (fact */ f, lett) in
	List.fold_left add_to_monomial pm lst
    | Const f -> (fact */ f, lett)
    | _ -> add_to_monomial m (e, 1) in
  mon_reduce nm


let pose_as_monomial x =
  match x with
    Product p -> p
  | Const c -> (c, [])
  | _ -> as_monomial x


let build_product a =
  let m = pose_as_monomial a in
  add_to_monomial_expr m


let raise_to_zero ex =
  if ex = Const zero then raise Indeterminate_expr else Const unity


let build_power b e =
  match (b, e) with
    (Const x, Const y) when y <> zero -> Const ( x **/ y )
  | (_, Const Int 0) -> raise_to_zero b
  | (_, Const Int 1) -> b
  | (_, Const Int ie) -> Product (unity, [b, ie])
  | _ -> Power (b, e)


let similar a b =
  let test (b1, e1) (b2, e2) = 
    let c = cmp b1 b2 in
    if c = 0 then e2 - e1 else c in
  let cfr = cmp_lists test a b in
  cfr


let rec mlist_add_mon mlst (f, pp) =
  match mlst with
    [] -> [f, pp]
  | (fx, px)::tail ->
      let cfr = similar px pp in
      if cfr < 0 then
        (fx, px) :: mlist_add_mon tail (f, pp)
      else
        if cfr > 0 then (f, pp) :: mlst
        else
	  let coeff = f +/ fx in
	  if coeff = zero then tail else (coeff, pp) :: tail


let poly_add_mon (noto, ml) m =
  noto, mlist_add_mon ml m


let poly_reduce p =
  match p with
    (c, []) -> Const c
  | (Int 0, [m]) -> mon_reduce m
  | _ -> Sum p


let add_to_poly_expr p x =
  let (noto, ml) = p in
  match x with
    Const c -> noto +/ c, ml
  | Product m -> poly_add_mon p m
  | Sum (nx, mlx) ->
      List.fold_left poly_add_mon (noto +/ nx, ml) mlx
  | _ -> poly_add_mon p (as_monomial x)


let build_sum a x =
  let pol =
    match a with
      Sum s -> s
    | Product p -> (zero, [p])
    | Const c -> (c, [])
    | z -> (zero, [as_monomial z]) in
  let neo = add_to_poly_expr pol x in
  poly_reduce neo

let minus_expr x =
  match x with
    Const c -> Const (c_minus c)
  | Product (f, lett) -> Product (c_minus f, lett)
  | Sum (n, ml) -> 
      let minfun (f1, p1) = (c_minus f1, p1) in
      Sum (c_minus n, List.map minfun ml)
  | z -> Product (Int (-1), [z, 1])
