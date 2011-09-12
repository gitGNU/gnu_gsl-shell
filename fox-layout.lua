
local bor = (require 'bit').bor

local LAYOUT = {}

LAYOUT.NORMAL      = 0                                   -- Default layout mode
LAYOUT.SIDE_TOP    = 0                                   -- Pack on top side (default)
LAYOUT.SIDE_BOTTOM = 0x00000001                          -- Pack on bottom side
LAYOUT.SIDE_LEFT   = 0x00000002                          -- Pack on left side
LAYOUT.SIDE_RIGHT  = bor(LAYOUT.SIDE_LEFT,LAYOUT.SIDE_BOTTOM) -- Pack on right side
LAYOUT.FILL_COLUMN = 0x00000001                          -- Matrix column is stretchable
LAYOUT.FILL_ROW    = 0x00000002                          -- Matrix row is stretchable
LAYOUT.LEFT        = 0                                   -- Stick on left (default)
LAYOUT.RIGHT       = 0x00000004                          -- Stick on right
LAYOUT.CENTER_X    = 0x00000008                          -- Center horizontally
LAYOUT.FIX_X       = bor(LAYOUT.RIGHT,LAYOUT.CENTER_X)   -- X fixed
LAYOUT.TOP         = 0                                   -- Stick on top (default)
LAYOUT.BOTTOM      = 0x00000010                          -- Stick on bottom
LAYOUT.CENTER_Y    = 0x00000020                          -- Center vertically
LAYOUT.FIX_Y       = bor(LAYOUT.BOTTOM,LAYOUT.CENTER_Y)  -- Y fixed
LAYOUT.DOCK_SAME   = 0                                   -- Dock on same galley if it fits
LAYOUT.DOCK_NEXT   = 0x00000040                          -- Dock on next galley
LAYOUT.RESERVED_1  = 0x00000080
LAYOUT.FIX_WIDTH   = 0x00000100                          -- Width fixed
LAYOUT.FIX_HEIGHT  = 0x00000200                          -- height fixed
LAYOUT.MIN_WIDTH   = 0                                   -- Minimum width is the default
LAYOUT.MIN_HEIGHT  = 0                                   -- Minimum height is the default
LAYOUT.FILL_X      = 0x00000400                          -- Stretch or shrink horizontally
LAYOUT.FILL_Y      = 0x00000800                          -- Stretch or shrink vertically
LAYOUT.FILL        = bor(LAYOUT.FILL_X,LAYOUT.FILL_Y)    -- Stretch or shrink in both directions
LAYOUT.EXPLICIT    = bor(LAYOUT.FIX_X,LAYOUT.FIX_Y,LAYOUT.FIX_WIDTH,LAYOUT.FIX_HEIGHT)   -- Explicit placement

return LAYOUT
