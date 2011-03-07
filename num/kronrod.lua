
local globals_report = {
   __index = function(t, k) 
		error(string.format('reference to undefined variable %s', k))
	     end,
   __newindex = function (t, k, v)
		   error(string.format('reference to undefined variable %s', k))
		end
}

local sin, sqrt, abs, floor = math.sin, math.sqrt, math.abs, math.floor

local globals = {}
setmetatable(globals, globals_report)
setfenv(1, globals)

local function abwe1(n, m, eps, coef2, even, b, x)
   --
   -- Purpose:
   --
   --   ABWE1 calculates a Kronrod abscissa and weight.
   --
   -- Licensing:
   --
   --   This code is distributed under the GNU LGPL license.
   --
   -- Modified:
   --
   --   03 August 2010
   --
   -- Author:
   --
   --   Original FORTRAN77 version by Robert Piessens, Maria Branders.
   --   C version by John Burkardt.
   --   Lua version by Francesco Abbate.
   --
   -- Reference:
   --
   --   Robert Piessens, Maria Branders,
   --   A Note on the Optimal Addition of Abscissas to Quadrature Formulas
   --   of Gauss and Lobatto,
   --   Mathematics of Computation,
   --   Volume 28, Number 125, January 1974, pages 135-139.
   --
   -- Parameters:
   --
   --   Input, int N, the order of the Gauss rule.
   --
   --   Input, int M, the value of ( N + 1 ) / 2.
   --
   --   Input, double EPS, the requested absolute accuracy of the
   --   abscissas.
   --
   --   Input, double COEF2, a value needed to compute weights.
   --
   --   Input, int EVEN, is TRUE if N is even.
   --
   --   Input, double B[M+1], the Chebyshev coefficients.
   --
   --   Input/output, double *X; on input, an estimate for
   --   the abscissa, and on output, the computed abscissa.
   --
   --   Output, double *W, the weight.
   --
   local ai, d0, d1, d2, fd

   local ka = (x == 0 and 1 or 0)

   --  Iterative process for the computation of a Kronrod abscissa.

   for iter =1, 50 do
      local b0, b1, b2 = 0, 0, b[m+1]
      local yy = 4 * x * x - 2
      d1 = 0

      local dif
      if even then
	 ai = m + m + 1
	 d2 = ai * b[m+1]
	 dif = 2
      else
	 ai = m + 1
	 d2 = 0
	 dif = 1
      end

      for k = 1, m do
	 local i = m - k + 1
	 ai = ai - dif
	 b0, b1 = b1, b2
	 d0, d1 = d1, d2
	 b2 = yy * b1 - b0 + b[i]
	 if not even then i = i + 1 end
	 d2 = yy * d1 - d0 + ai * b[i]
      end

      local f = (even and x * (b2 - b1) or 0.5 * (b2 - b0))
      fd = (even and d2 + d1 or 4 * x * d2)

      --  Newton correction.

      local delta = f / fd
      x = x - delta

      if ka == 1 then break end

      if abs(delta) <= eps then ka = 1 end
   end

   --  Catch non-convergence.
   if ka ~= 1 then
      error("ABWE1 - Fatal error! Iteration limit reached.")
   end

   -- Computation of the weight.

   d0, d1 = 1, x
   ai = 0
   for k = 2, n do
      ai = ai + 1
      d2 = ( ( ai + ai + 1 ) * x * d1 - ai * d0 ) / ( ai + 1 )
      d0, d1 = d1, d2
   end

   local w = coef2 / ( fd * d2 )

   return x, w
end

local function abwe2(n, m, eps, coef2, even, b, x)
   -- Purpose:
   --
   --   ABWE2 calculates a Gaussian abscissa and two weights.
   --
   -- Licensing:
   --
   --   This code is distributed under the GNU LGPL license.
   --
   -- Modified:
   --
   --   03 August 2010
   --
   -- Author:
   --
   --   Original FORTRAN77 version by Robert Piessens, Maria Branders.
   --   C version by John Burkardt.
   --
   -- Reference:
   --
   --   Robert Piessens, Maria Branders,
   --   A Note on the Optimal Addition of Abscissas to Quadrature Formulas
   --   of Gauss and Lobatto,
   --   Mathematics of Computation,
   --   Volume 28, Number 125, January 1974, pages 135-139.
   --
   -- Parameters:
   --
   --   Input, int N, the order of the Gauss rule.
   --
   --   Input, int M, the value of ( N + 1 ) / 2.
   --
   --   Input, double EPS, the requested absolute accuracy of the
   --   abscissas.
   --
   --   Input, double COEF2, a value needed to compute weights.
   --
   --   Input, int EVEN, is TRUE if N is even.
   --
   --   Input, double B[M+1], the Chebyshev coefficients.
   --
   --   Input/output, double *X; on input, an estimate for
   --   the abscissa, and on output, the computed abscissa.
   --
   --   Output, double *W1, the Gauss-Kronrod weight.
   --
   --   Output, double *W2, the Gauss weight.
   local p0, p1, p2, pd2

   local ka = (x == 0 and 1 or 0)

   -- Iterative process for the computation of a Gaussian abscissa.

   for iter = 1, 50 do
      local pd0, pd1 = 0, 1

      p0, p1 = 1, x

      do
	 local ai = 0
	 for k = 2, n do
	    ai = ai + 1
	    p2 = ( ( ai + ai + 1 ) * x * p1 - ai * p0 ) / ( ai + 1 )
	    pd2 = ( ( ai + ai + 1 ) * ( p1 + x * pd1 ) - ai * pd0 ) / ( ai + 1 )
	    p0, p1 = p1, p2
	    pd0, pd1 = pd1, pd2
	 end
      end

      -- Newton correction.

      local delta = p2 / pd2
      x = x - delta

      if ka == 1 then break end
      
      if abs(delta) <= eps then ka = 1 end
   end

   --  Catch non-convergence.

   if ka ~= 1 then
      error("ABWE2 - Fatal error! Iteration limit reached.")
   end

   -- Computation of the weight.

   local an = n

   local w2 = 2 / ( an * pd2 * p0 )

   p1, p2 = 0, b[m+1]
   local yy = 4 * x * x - 2

   for k= 1, m do
      local i = m - k + 1
      p0, p1 = p1, p2
      p2 = yy * p1 - p0 + b[i]
   end

   local w1 = (even and w2 + coef2 / (pd2 * x * ( p2 - p1 )) or
	                w2 + 2 * coef2 / ( pd2 * ( p2 - p0 )))

   return x, w1, w2
end

local function kronrod (n, eps)
   -- Purpose:
   --
   --   KRONROD adds N+1 points to an N-point Gaussian rule.
   --
   -- Discussion:
   --
   --   This subroutine calculates the abscissas and weights of the 2N+1
   --   point Gauss Kronrod quadrature formula which is obtained from the
   --   N point Gauss quadrature formula by the optimal addition of N+1 points.
   --
   --   The optimally added points are called Kronrod abscissas.  The
   --   abscissas and weights for both the Gauss and Gauss Kronrod rules
   --   are calculated for integration over the interval [-1,+1].
   --
   --   Since the quadrature formula is symmetric with respect to the origin,
   --   only the nonnegative abscissas are calculated.
   --
   --   Note that the code published in Mathematics of Computation
   --   omitted the definition of the variable which is here called COEF2.
   --
   -- Storage:
   --
   --   Given N, let M = ( N + 1 ) / 2.
   --
   --   The Gauss-Kronrod rule will include 2*N+1 points.  However, by symmetry,
   --   only N + 1 of them need to be listed.
   --
   --   The arrays X, W1 and W2 contain the nonnegative abscissas in decreasing
   --   order, and the weights of each abscissa in the Gauss-Kronrod and
   --   Gauss rules respectively.  This means that about half the entries
   --   in W2 are zero.
   --
   --   For instance, if N = 3, the output is:
   --
   --   I      X               W1              W2
   --
   --   1    0.960491        0.104656         0.000000
   --   2    0.774597        0.268488         0.555556
   --   3    0.434244        0.401397         0.000000
   --   4    0.000000        0.450917         0.888889
   --
   --   and if N = 4, (notice that 0 is now a Kronrod abscissa)
   --   the output is
   --
   --   I      X               W1              W2
   --
   --   1    0.976560        0.062977        0.000000
   --   2    0.861136        0.170054        0.347855
   --   3    0.640286        0.266798        0.000000
   --   4    0.339981        0.326949        0.652145
   --   5    0.000000        0.346443        0.000000
   --
   -- Licensing:
   --
   --   This code is distributed under the GNU LGPL license.
   --
   -- Modified:
   --
   --   03 August 2010
   --
   -- Author:
   --
   --   Original FORTRAN77 version by Robert Piessens, Maria Branders.
   --   C version by John Burkardt.
   --
   -- Reference:
   --
   --   Robert Piessens, Maria Branders,
   --   A Note on the Optimal Addition of Abscissas to Quadrature Formulas
   --   of Gauss and Lobatto,
   --   Mathematics of Computation,
   --   Volume 28, Number 125, January 1974, pages 135-139.
   --
   -- Parameters:
   --
   --   Input, int N, the order of the Gauss rule.
   --
   --   Input, double EPS, the requested absolute accuracy of the
   --   abscissas.
   --
   --   Output, double X[N+1], the abscissas.
   --
   --   Output, double W1[N+1], the weights for
   --   the Gauss-Kronrod rule.
   --
   --   Output, double W2[N+1], the weights for
   --   the Gauss rule.
   local x, w1, w2 = {}, {}, {}
   
   local b = {}

   local m = floor (( n + 1 ) / 2)
   local even = ( 2 * m == n )

   local d, an = 2, 0

   for k = 1, n do
      an = an + 1
      d = d * an / ( an + 0.5 )
   end

   -- Calculation of the Chebyshev coefficients of the orthogonal polynomial.

   local tau = { (an + 2) / (an + an + 3) }
   b[m] = tau[1] - 1

   do
      local ak = an
      for l = 1, m-1 do
	 ak = ak + 2

	 local ccd = ak * ( ( ak + 3 ) * ( ak + 2 ) - an * ( an + 1 ) )
	 tau[l+1] = ((ak - 1) * ak - an * (an + 1)) * (ak + 2) * tau[l] / ccd
	 b[m-l] = tau[l+1]
	 
	 for ll = 1, l do
	    b[m-l] = b[m-l] + tau[ll] * b[m-l+ll]
	 end
      end
   end

   b[m+1] = 1

   -- Calculation of approximate values for the abscissas.

   local bb = sin ( 1.570796 / ( an + an + 1 ) )
   local x1 = sqrt ( 1 - bb * bb )
   local s = 2 * bb * x1
   local c = sqrt ( 1 - s * s )
   local coef = 1 - ( 1 - 1 / an ) / ( 8 * an * an )

   -- Coefficient needed for weights.

   -- COEF2 = 2^(2*n+1) * n * n / (2n+1)

   local coef2 = 2 / ( 2 * n + 1 )
   for i = 1, n do
      coef2 = coef2 * 4 * i / ( n + i )
   end

   -- Calculation of the K-th abscissa (a Kronrod abscissa) and the
   -- corresponding weight.

   for k = 1, n, 2 do
      x[k], w1[k] = abwe1 (n, m, eps, coef2, even, b, coef * x1)
      w2[k] = 0

      local y = x1
      x1 = y * c - bb * s
      bb = y * s + bb * c

      local xx = (k == n and 0 or coef * x1)

      -- Calculation of the K+1 abscissa (a Gaussian abscissa) and the
      -- corresponding weights.

      x[k+1], w1[k+1], w2[k+1] = abwe2 (n, m, eps, coef2, even, b, xx)

      y = x1
      x1 = y * c - bb * s
      bb = y * s + bb * c
   end

   -- If N is even, we have one more Kronrod abscissa to compute,
   -- namely the origin.

   if even then
      x[n+1], w1[n+1] = abwe1 ( n, m, eps, coef2, even, b, 0)
      w2[n+1] = 0
   end

   return x, w1, w2
end

return kronrod
