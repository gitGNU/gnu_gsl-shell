
local bor = (require 'bit').bor

local OPT = { }

OPT.ICON_UNDER_TEXT      = 0			      -- Icon appears under text
OPT.ICON_AFTER_TEXT      = 0x00080000		      -- Icon appears after text (to its right)
OPT.ICON_BEFORE_TEXT     = 0x00100000		      -- Icon appears before text (to its left)
OPT.ICON_ABOVE_TEXT      = 0x00200000		      -- Icon appears above text
OPT.ICON_BELOW_TEXT      = 0x00400000		      -- Icon appears below text
OPT.TEXT_OVER_ICON       = OPT.ICON_UNDER_TEXT	      -- Same as ICON_UNDER_TEXT
OPT.TEXT_AFTER_ICON      = OPT.ICON_BEFORE_TEXT	      -- Same as ICON_BEFORE_TEXT
OPT.TEXT_BEFORE_ICON     = OPT.ICON_AFTER_TEXT	      -- Same as ICON_AFTER_TEXT
OPT.TEXT_ABOVE_ICON      = OPT.ICON_BELOW_TEXT	      -- Same as ICON_BELOW_TEXT
OPT.TEXT_BELOW_ICON      = OPT.ICON_ABOVE_TEXT	      -- Same as ICON_ABOVE_TEXT

OPT.JUSTIFY_NORMAL       = 0			      -- Default justification is centered text
OPT.JUSTIFY_CENTER_X     = 0			      -- Contents centered horizontally
OPT.JUSTIFY_LEFT         = 0x00008000		      -- Contents left-justified
OPT.JUSTIFY_RIGHT        = 0x00010000		      -- Contents right-justified
OPT.JUSTIFY_HZ_APART     = bor(OPT.JUSTIFY_LEFT,OPT.JUSTIFY_RIGHT)  -- Combination of JUSTIFY_LEFT & JUSTIFY_RIGHT
OPT.JUSTIFY_CENTER_Y     = 0			      -- Contents centered vertically
OPT.JUSTIFY_TOP          = 0x00020000		      -- Contents aligned with label top
OPT.JUSTIFY_BOTTOM       = 0x00040000		      -- Contents aligned with label bottom
OPT.JUSTIFY_VT_APART     = bor(OPT.JUSTIFY_TOP,OPT.JUSTIFY_BOTTOM)   -- Combination of JUSTIFY_TOP & JUSTIFY_BOTTOM

return OPT
