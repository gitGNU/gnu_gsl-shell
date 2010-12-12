
local opcode = require 'opcode'

local fop_mnemonic = {add= 0xc1, sub= 0xe9, mul= 0xc9, div= 0xf9}

local codegen

local function codegen_binop(op)
   local mnem = fop_mnemonic[op]
   return function(code, e)
	     local a, b = e[1], e[2]
	     codegen(code, a)
	     codegen(code, b)
	     opcode.emit(code, 0xde, mnem)
	  end
end

local function codegen_var(code, v)
   local n = v.arg_index
   opcode.emit(code, 0xdd, 0x45, 0x08 * n)
end

local function codegen_unm(code, e)
   local o = e[1]
   codegen(code, o)
   opcode.emit(code, 0xd9, 0xe0)
end

local codegen_table = {add= codegen_binop 'add', mul= codegen_binop 'mul',
		       sub= codegen_binop 'sub', div= codegen_binop 'div',
		       unm= codegen_unm, var= codegen_var}

codegen = function(code, e)
	     if type(e) == 'number' then
		local mem_addr = opcode.register_constant(code, e)
		opcode.emit(code, 0xd9, 0x05)
		opcode.emit_address(code, mem_addr)
	     else
		local cf = codegen_table[e.tag]
		cf(code, e)
	     end
	  end

local function emit_header(code)
   opcode.emit(code, 0x55)       -- push %ebp
   opcode.emit(code, 0x89, 0xe5) -- mov  %esp,%ebp
end

local function emit_footer(code)
   opcode.emit(code, 0x5d)       -- pop %ebp
   opcode.emit(code, 0xc3)       -- ret
end

function codegen_func(ex, args)
   for i, v in ipairs(args) do v.arg_index = i end

   local code = opcode.new(#args)
   emit_header(code)
   codegen(code, ex)
   emit_footer(code)
   return code
end
