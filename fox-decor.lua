
local bor = (require 'bit').bor

local DECOR = {}

DECOR.NONE        = 0                                  -- Borderless window
DECOR.TITLE       = 0x00020000                         -- Window title
DECOR.MINIMIZE    = 0x00040000                         -- Minimize button
DECOR.MAXIMIZE    = 0x00080000                         -- Maximize button
DECOR.CLOSE       = 0x00100000                         -- Close button
DECOR.BORDER      = 0x00200000                         -- Border
DECOR.SHRINKABLE  = 0x00400000                         -- Window can become smaller
DECOR.STRETCHABLE = 0x00800000                         -- Window can become larger
DECOR.RESIZE      = bor(DECOR.SHRINKABLE,DECOR.STRETCHABLE) -- Resize handles
DECOR.MENU        = 0x01000000                         -- Window menu
DECOR.ALL         = bor(DECOR.TITLE,DECOR.MINIMIZE,DECOR.MAXIMIZE,DECOR.CLOSE,DECOR.BORDER,DECOR.SHRINKABLE,DECOR.STRETCHABLE,DECOR.MENU)

return DECOR
