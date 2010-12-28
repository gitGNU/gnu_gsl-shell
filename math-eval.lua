
function make_math()
   local mt = {}

   local function is_operation(op, a)
      return type(a) == 'table' and a.tag == op
   end

   local function get_operands(op, a)
      local ls = {}
      if is_operation(op, a) then
	 for i, v in ipairs(a) do ls[i] = v end
      else
	 ls[1] = a
      end
      return ls
   end

   local function append(a, b)
      local n = #a
      for i, v in ipairs(b) do a[n+i] = v end
   end

   local function make_assoc(tag_name)
      return function(a, b)
		local e = {tag= tag_name}
		local as = get_operands(tag_name, a)
		local bs = get_operands(tag_name, b)
		append(e, as)
		append(e, bs)
		setmetatable(e, mt)
		return e
	     end
   end

   local function unm(a)
      local e = {tag= 'unm', a}
      setmetatable(e, mt)
      return e
   end

   local function make_ncomm(tag_name)
      return function(a, b)
		local e = {tag= tag_name, a, b}
		setmetatable(e, mt)
		return e
	     end
   end

   local methods = {__add= make_ncomm 'add', __mul= make_ncomm 'mul',
		    __sub= make_ncomm 'sub', __div= make_ncomm 'div',
		    __unm= unm}

   for k, v in pairs(methods) do mt[k] = v end

   local function var(s)
      local t = {tag='var', s}
      setmetatable(t, mt)
      return t
   end

   return var
end

var = make_math()
