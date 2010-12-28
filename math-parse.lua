-- parse.js
-- Parser for Simplified JavaScript written in Simplified JavaScript
-- From Top Down Operator Precedence
-- http:--javascript.crockford.com/tdop/index.html
-- Douglas Crockford
-- 2010-06-26

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

local function make_parse()
   local scope
   local symbol_table = {}
   local token
   local tokens
   local token_nr

   local itself = function (this) return this end

   local function advance(id)
      local a, o, t, v
      if id and token.id ~= id then
	 error(strinf.format("Expected '%s'", id))
      end

      if token_nr >= tokens.length then
	 token = symbol_table["(end)"]
	 return
      end

      t = tokens[token_nr]
      token_nr += 1
      v = t.value
      a = t.type
      
      if a == "name" then
	 o = scope.find(v)
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
      left = t:nud()
      while rbp < token.lbp do
	 t = token
	 advance()
	 left = t:led(left)
      end
      return left
   end

   local original_symbol = {
      nud= function () error "Undefined." end,
      led= function (left) error "Missing operator." end
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
	 s.id = s.value = id
	 s.lbp = bp
	 symbol_table[id] = s
      end
      return s
   end

   local function constant(s, v)
      local x = symbol(s)
      x.nud = function (this)
		 scope.reserve(this)
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
			 scope.reserve(this)
			 this.first = expression(70)
			 this.arity = "unary"
			 return this
		      end)
      return s
   end

   symbol("(end)")
   symbol("(name)")
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
		scope.define(token)
		this.name = token.value
		advance()
	     end
	     advance("(")
	     if token.id ~= ")" then
		while true then
		   if token.arity ~= "name" then
		      token_error(token, "Expected a parameter name.")
		   end
		   scope.define(token)
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
	     this.second = expression()
	     advance("end")
	     this.arity = "function"
	     scope.pop()
	     return this
	  end)

   return function (source)
	     tokens = string_tokens(source)
	     token_nr = 0
	     new_scope()
	     advance()
	     local s = statements()
	     advance("(end)")
	     scope.pop()
	     return s
	  end
end
