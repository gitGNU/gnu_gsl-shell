#ifndef GUI_TYPES_H
#define GUI_TYPES_H

namespace gui {

  // TODO: use LuaJIT2 C declarations parser to obtain the enum integer
  // values to be used in Lua.
  enum typeid_e {
    main_window = 1,
    horizontal_frame,
    vertical_frame,
    label,
    text_field,
    button,
    canvas,
    menu_bar,
    menu_title,
    menu_pane,
    menu_command,
    dialog_box,
  };

  enum method_e {
    close         = 0,
    get_text      = 1,
    set_text      = 2,
    get_size      = 3,
    enable        = 4,
    disable       = 5,
  };

}

#endif
