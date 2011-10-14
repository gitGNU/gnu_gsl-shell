#include <string.h>

#include "gui_element.h"

int text_field::handle(lua_State* L, FXApp* app, int method_id, err& st)
{
  FXTextField *tf = (FXTextField*) m_widget;

  switch (method_id) {
  case gui::set_text:
    {
      const char *text = lua_tostring(L, 4);
      tf->setText(text);
      return 0;
    }
  case gui::get_text:
    {
      FXString text = tf->getText();
      lua_pushstring(L, text.text());
      return 1;
    }
  default:
    /* */ ;
  }

  return gui_window::handle(L, app, method_id, st);
}

int combo_box::handle(lua_State* L, FXApp* app, int method_id, err& st)
{
  FXComboBox* w = (FXComboBox*) m_widget;

  switch (method_id) {
  case gui::append_item:
    {
      const char *text = lua_tostring(L, 4);
      w->appendItem(text);
      return 0;
    }
  case gui::set_nb_visible:
    {
      int n = lua_tointeger(L, 4);
      w->setNumVisible(n);
      return 0;
    }
  default:
    /* */ ;
  }

  return gui_window::handle(L, app, method_id, st);
}

int gui_window::handle(lua_State* L, FXApp* app, int method_id, err& st)
{
  switch (method_id) {
  case gui::get_size:
    {
      FXWindow *win = (FXWindow*) m_widget;
      int w = win->getWidth(), h = win->getHeight();
      lua_pushinteger(L, w);
      lua_pushinteger(L, h);
      return 2;
    }
  default:
    /* */ ;
  }

  return gui_drawable::handle(L, app, method_id, st);
}
