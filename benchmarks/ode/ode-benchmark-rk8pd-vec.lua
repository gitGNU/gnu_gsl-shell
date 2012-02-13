
-- Benchmark of ODE integration functions.
-- The benchmark integrates the Van der Pol differential equation
-- using over a long time interval

local format = string.format

function f_vanderpol_gen(mu)
   return function(t, y, dydt)
	     local x, y = y.data[0], y.data[1]
	     dydt.data[0] = y
	     dydt.data[1] = -x + mu * y * (1-x^2)
	  end
end

local results = {}

local f = f_vanderpol_gen(10.0)
local s = num.odevec {N= 2, eps_abs= 1e-8, method= 'rk8pd'}
local y0 = matrix.vec {1, 0}
local t0, t1, h0 = 0, 1000, 0.01
local evol = s.evolve
for k=1, 10 do
   s:init(t0, h0, f, y0)
   while s.t < t1 do
      evol(s, f, t1)
   end
   results[#results+1] = format('%g %g %g', s.t, s.y[1], s.y[2])
end

for i, line in ipairs(results) do
   echo(i, line)
end
