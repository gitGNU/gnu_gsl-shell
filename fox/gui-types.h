#ifndef GUI_TYPES_H
#define GUI_TYPES_H

namespace gui {

  enum typeid_e {
    main_window      = 0,
    horizontal_frame = 1,
    vertical_frame   = 2,
    label            = 3,
    text_field       = 4,
    button           = 5,
    canvas           = 6,
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
