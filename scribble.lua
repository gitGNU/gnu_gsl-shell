local UI = require 'gui'

local DC    = require 'fox-dc'
local EVENT = require 'fox-event'

local mdflag = false

local function clear_rect(w, x, y, width, height)
   w:getDC(w:element'canvas')
   w:draw(DC.SET_FOREGROUND, DC.RGB(255, 255, 255))
   w:draw(DC.FILL_RECTANGLE, x, y, width, height)
end

local function do_paint(w)
   clear_rect(w, w:event(EVENT.RECT))
end

local function do_clear(w)
   clear_rect(w, 0, 0, w:handle(w:element'canvas', 'get_size'))
end

local function on_move(w)
   if mdflag then
      local x1, y1 = w:event(EVENT.LAST_X, EVENT.LAST_Y)
      local x2, y2 = w:event(EVENT.WIN_X,  EVENT.WIN_Y)
      w:getDC(w:element'canvas')
      w:draw(DC.SET_FOREGROUND, DC.RGB(255, 0, 0))
      w:draw(DC.DRAW_LINE, x1, y1, x2, y2)
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

	 UI.Button { text = 'Clear', onCommand = do_clear },
	 UI.Button { 
	    text = 'Exit', 
	    onCommand = function(w) w:handle(w:self(), 'close') end 
	 }
      }
   }
}

w = UI.Create(ctors)
