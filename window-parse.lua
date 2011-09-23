
local UI = require 'gui'

local function coucou(w)
   local id = w:element('plot_tf')
   local txt = w:handle(id, 'get_text')
   w:handle(id, 'set_text', txt .. 'coucou')
   echo('>>>', w) 
   return 1
end

ctors = UI.MainWindow {
   title = "Test Window",
   width  = 640,
   height = 480,

   UI.VerticalFrame {
      layout = {'FILL_X', 'FILL_Y'},

      UI.HorizontalFrame {
	 layout = {'FILL_X'},

	 UI.TextField {
	    name = 'plot_tf',
	    columns = 24,
--	    onCommand = my_on_command,
	 },

	 UI.Button {
	    text = "Plot",
	    onCommand = coucou,
	 },	    
      },

      UI.Canvas {
	 onPaint = my_paint_function,
      },
   },
}

w = UI.Create(ctors)

-- local f = loadfile('test_window.lua')
