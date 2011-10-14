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
    radio_button,
    check_button,
    combo_box,
  };

  enum method_e {
    close = 1,
    get_text,
    set_text,
    get_size,
    enable,
    disable,
    check,
    uncheck,
    set_unknown,
    append_item,
    set_nb_visible,
  };

}

#endif
