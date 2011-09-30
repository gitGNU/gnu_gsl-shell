local UI = require 'gui'

local DC    = require 'fox-dc'
local EVENT = require 'fox-event'
local OP    = require 'fox-method'

local dirty, mdflag = false, false

-- Since the canvas id is frequently used we store its value in a local variable.
-- The actual value is retrieved only after the creation of the window
local canvas_id

local function clear_rect(w, x, y, width, height)
   w:getDC(canvas_id)
   w:draw(DC.SET_FOREGROUND, DC.RGB(255, 255, 255))
   w:draw(DC.FILL_RECTANGLE, x, y, width, height)
end

local function do_paint(w)
   clear_rect(w, w:event(EVENT.RECT))
end

local function do_clear(w)
   clear_rect(w, 0, 0, w:handle(canvas_id, OP.GET_SIZE))
   dirty = false
end

local function on_move(w)
   if mdflag then
      local x1, y1 = w:event(EVENT.LAST_X, EVENT.LAST_Y)
      local x2, y2 = w:event(EVENT.WIN_X,  EVENT.WIN_Y)
      w:getDC(canvas_id)
      w:draw(DC.SET_FOREGROUND, DC.RGB(255, 0, 0))
      w:draw(DC.DRAW_LINE, x1, y1, x2, y2)
      dirty = true
   end
end

ctors = UI.MainWindow {
   title = "Test Window",
   width  = 640,
   height = 480,

   UI.HorizontalFrame {
      layout = {'SIDE_TOP', 'FILL_X', 'FILL_Y'},

      UI.VerticalFrame {
	 layout = {'FILL_X', 'FILL_Y', 'TOP', 'LEFT'},
	 style = {'SUNKEN'},
	 
	 UI.Label {
	    layout = {'FILL_X'},
	    text = 'Canvas Frame'
	 },

	 UI.Canvas {
	    name = 'canvas',
	    layout = {'FILL_X', 'FILL_Y', 'FILL_COLUMN', 'FILL_ROW'},
	    style = {'SUNKEN'},
	    onPaint = do_paint,
	    onLeftButtonPress = function() mdflag = true end,
	    onLeftButtonRelease = function() mdflag = false end,
	    onMotion = on_move,
	 }
      },

      UI.VerticalFrame {
	 position = 'TOP/LEFT',
	 layout = {'FILL_Y'},
	 style = {'SUNKEN'},
	 
	 UI.Label {
	    layout = {'FILL_X'},
	    text = "Button Frame",
	 },

	 UI.Button {
	    text = 'Clear', 
	    name = 'clear_bt',
	    onCommand = do_clear, 
	    onUpdate = function(w)
			  local id = w:element'clear_bt'
			  w:handle(id, dirty and OP.ENABLE or OP.DISABLE)
		       end,
	 },

	 UI.Button { 
	    text = 'Exit', 
	    onCommand = function(w) w:handle(w:self(), OP.CLOSE) end 
	 }
      }
   }
}

win = UI.Create(ctors)

canvas_id = win:element'canvas'
