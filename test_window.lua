
MainWindow {
   title = "Test Window",
   width  = 640,
   height = 480,

   VerticalFrame {
      layout = {'FILL_X', 'FILL_Y'},

      HorizontalFrame {
	 layout = {'FILL_X'},

	 TextField {
	    columns = 24,
--	    onCommand = my_on_command,
	 },

	 Button {
	    text = "Plot",
	    onCommand = my_plot_function,
	 },	    
      },

      Canvas {
	 onPaint = my_paint_function,
      },
   },
}
