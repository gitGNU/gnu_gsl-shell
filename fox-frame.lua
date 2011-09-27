
local bor = (require 'bit').bor

local FRAME = {}

FRAME.NONE   = 0                                     -- Default is no frame
FRAME.SUNKEN = 0x00001000                            -- Sunken border
FRAME.RAISED = 0x00002000                            -- Raised border
FRAME.THICK  = 0x00004000                            -- Thick border
FRAME.GROOVE = FRAME.THICK                           -- A groove or etched-in border
FRAME.RIDGE  = bor(FRAME.THICK,FRAME.RAISED,FRAME.SUNKEN) -- A ridge or embossed border
FRAME.LINE   = bor(FRAME.RAISED,FRAME.SUNKEN)             -- Simple line border
FRAME.NORMAL = bor(FRAME.SUNKEN,FRAME.THICK)              -- Regular raised/thick border

return FRAME
