
function testf(x, y)
   return (x + y) * (x - y)
end

function test2(x, y)
   return x*x - y*y
end

dofile 'math-eval.lua'

x = var 'x'
y = var 'y'

e = test2(x, y)

dofile 'codegen.lua'
f = codegen_func(e, {x, y})

r = opcode.call(f, 5, 1.2)
print(r)

