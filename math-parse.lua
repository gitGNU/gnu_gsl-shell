-- parse.js
-- Parser for Simplified JavaScript written in Simplified JavaScript
-- From Top Down Operator Precedence
-- http:--javascript.crockford.com/tdop/index.html
-- Douglas Crockford
-- 2010-06-26

dofile 'math-tokens.lua'

local jsobj_index = function(obj, k)
		       local p = obj.__proto
		       if p then return p[k] end
		    end
		       
local jsobj_mt = {__index= jsobj_index}

local function jsobject(o)
   local t = {__proto= o}
   setmetatable(t, jsobj_mt)
   return t
end

local function push(ls, e)
   ls[#ls+1] = e
end

function define_parser()
   local scope
   local global_scope
   local symbol_table = {}
   local token
   local tokens
   local token_nr

   local itself = function (this) return this end

   local original_scope = {
      define= function (this, n)
		 local t = this.def[n.value]
		 if type(t) == "table" then
		    error(t.reserved and "Already reserved." or "Already defined.")
		 end
		 this.def[n.value] = n
		 -- n.reserved = false
		 n.nud      = itself
		 -- n.led      = nil
		 -- n.std      = nil
		 n.lbp      = 0
		 n.scope    = scope
		 return n
	      end,
      find= function (this, n)
	       local e, o = this
	       while true do
		  o = e.def[n]
		  if o and type(o) ~= 'function' then
		     return e.def[n]
		  end
		  e = e.parent
		  if not e then
		     o = symbol_table[n]
		     return (o and type(o) ~= 'function') and o or symbol_table["(name)"]
		  end
	       end
	    end,
      pop= function (this)
	      scope = this.parent
	   end,
      reserve= function (this, n)
		  if n.arity ~= "name" or n.reserved then
		     return
		  end
		  local t = this.def[n.value]
		  if t then
		     if t.reserved then
			return
		     end
		     if t.arity == "name" then
			error "Already defined."
		     end
		  end
		  this.def[n.value] = n
		  n.reserved = true
	       end,
   }

   local function new_scope()
      local s = scope
      scope = jsobject(original_scope)
      scope.def = {}
      scope.parent = s
      return scope
   end

   local function advance(id)
      local a, o, t, v
      if id and token.id ~= id then
	 error(string.format("Expected '%s'", id))
      end

      if token_nr > #tokens then
	 token = symbol_table["(end)"]
	 return
      end

      t = tokens[token_nr]
      token_nr = token_nr + 1
      v = t.value
      a = t.type
      
      if a == "name" then
	 o = scope:find(v)
	 -- if o.value == '(name)' then
	 --    o = global_scope:define(t)
	 --    print('FROM GLOBAL SCOPE', o)
	 -- end
      elseif a == "operator" then
	 o = symbol_table[v]
	 if not o then error "Unknown operator." end
      elseif a == "string" or a == "number" then
	 o = symbol_table["(literal)"]
	 a = "literal"
      else
	 error "Unexpected token."
      end

      token = jsobject(o)
      token.from  = t.from
      token.to    = t.to
      token.value = v
      token.arity = a
      return token
   end

   local function expression(rbp)
      local left
      local t = token
      advance()
      left = t:nud(true)
      while rbp < token.lbp do
	 t = token
	 advance()
	 left = t:led(left)
      end
      return left
   end

   local original_symbol = {
      nud= function (this, weak) 
	      if weak then return this else error("NUD: undefined.") end
	   end,
      led= function (left) error("LED: missing operator.") end
   }

   local function symbol(id, bp)
      local s = symbol_table[id]
      bp = bp or 0
      if s then
	 if bp >= s.lbp then
	    s.lbp = bp
	 end
      else
	 s = jsobject(original_symbol)
	 s.id, s.value = id, id
	 s.lbp = bp
	 symbol_table[id] = s
      end
      return s
   end

   local function constant(s, v)
      local x = symbol(s)
      x.nud = function (this)
		 scope:reserve(this)
		 this.value = symbol_table[this.id].value
		 this.arity = "literal"
		 return this
	      end
      x.value = v
      return x
   end

   local function infix(id, bp, led)
      local s = symbol(id, bp)
      s.led = led or (function (this, left)
			 this.first = left
			 this.second = expression(bp)
			 this.arity = "binary"
			 return this 
		      end)
      return s
   end

   local function prefix(id, nud)
      local s = symbol(id)
      s.nud = nud or (function (this)
			 scope:reserve(this)
			 this.first = expression(70)
			 this.arity = "unary"
			 return this
		      end)
      return s
   end

   symbol(",")
   symbol("(end)")
   symbol("(name)")
   symbol("(var)")
   symbol(")")

   symbol("end")
    
   symbol("(literal)").nud = itself

   infix("+", 50)
   infix("-", 50)

   infix("*", 60)
   infix("/", 60)

   prefix("-")

   prefix("(", function ()
		  local e = expression(0)
		  advance ")"
		  return e
	       end)

   prefix("function", 
	  function (this)
	     local a = {}
	     new_scope()
	     if token.arity == "name" then
		scope:define(token)
		this.name = token.value
		advance()
	     end
	     advance("(")
	     if token.id ~= ")" then
		while true do
		   if token.arity ~= "name" then
		      token_error(token, "Expected a parameter name.")
		   end
		   scope:define(token)
		   push(a, token)
		   advance()
		   if token.id ~= "," then
		      break
		   end
		   advance(",")
		end
	     end

	     this.first = a
	     advance(")")
	     this.second = expression(0)
	     advance("end")
	     this.arity = "function"
	     scope:pop()
	     return this
	  end)

   return function (source)
	     tokens = string_tokens(source)
	     token_nr = 1
	     global_scope = new_scope()
	     advance()
	     local e = expression(0)
	     advance("(end)")
	     scope:pop()
	     return e
	  end
end

local function brackets(ls)
   return string.format('(%s)', table.concat(ls, ' '))
end

function lisp_print(e)
   if e.arity == 'binary' then
      local t = {e.value, lisp_print(e.first), lisp_print(e.second)}
      return brackets(t)
   elseif e.arity == "literal" then 
      return tostring(e.value)
   elseif e.arity == "name" then 
      return e.value
   elseif e.arity == "function" then
      local args = {}
      for _, a in ipairs(e.first) do push(args, a.value) end
      local t = {'lambda', brackets(args), lisp_print(e.second)}
      return brackets(t)
   end
end

parser = define_parser()
