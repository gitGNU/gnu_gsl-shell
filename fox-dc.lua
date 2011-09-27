
local bit = require 'bit'

local DC = {
   SET_FOREGROUND = 1,
   DRAW_LINE      = 2, 
   FILL_RECTANGLE = 3,
}

function DC.RGB(r, g, b)
--   r, g, b = bit.band(r, 0xFF), bit.band(g, 0xFF), bit.band(b, 0xFF)
   return bit.bor(bit.lshift(r, 24), bit.lshift(g, 16), bit.lshift(b, 8), 0xFF)
end

return DC
