
-- win = window('h(v..)(v..)')
win = window()
w = glplot()

local xmin, xmax = -20, 20
local ymin, ymax = -20, 20
local nx, ny = 20, 20

local fecs = |x,y| exp(-0.3*2*(x^2+y^2)) * (cos(pi*x) + cos(pi*y))
local fzero = |x,y| x*exp(-2*x^2-2*y^2)

x = new(nx, 1, |i| xmin + (xmax - xmin) * (i-1) / (nx-1))
y = new(ny, 1, |j| ymin + (ymax - ymin) * (j-1) / (ny-1))
z = new(nx, ny, |i,j| fzero(x[i]/10, y[j]/10))

local v1, v2 = new(3,1), new(3,1)

local function set_normal (w)
  local x = v1[2]*v2[3] - v1[3]*v2[2]
  local y = v1[3]*v2[1] - v1[1]*v2[3]
  local z = v1[1]*v2[2] - v1[2]*v2[1]

  local n = sqrt(x*x+y*y+z*z);

  w:normal(x/n, y/n, z/n)
end

w:start 'lightblue'

for j=1, ny-1 do
   w:vbegin 'QUAD_STRIP'

   v1[1] = x[2] - x[1]
   v1[2] = 0
   v1[3] = z:get(2, j) - z:get(1,j)

   v2[1] = x[2] - x[1];
   v2[2] = y[j+1] - y[j];
   v2[3] = z:get(2,j+1) - z:get(1,j)

   set_normal(w)

   w:vertex(x[1], y[j],   z:get(1,j)  )
   w:vertex(x[1], y[j+1], z:get(1,j+1))

   for i=1, nx-1 do
      v1[1] = x[i+1] - x[i]
      v1[2] = 0
      v1[3] = z:get(i+1, j) - z:get(i,j)

      v2[1] = x[i+1] - x[i];
      v2[2] = y[j+1] - y[j];
      v2[3] = z:get(i+1,j+1) - z:get(i,j)

      set_normal(w)

      w:vertex(x[i+1], y[j],   z:get(i+1,j)  )
      w:vertex(x[i+1], y[j+1], z:get(i+1,j+1))

   end
   w:vend()
end

w:close()
win:attach(w, '')
