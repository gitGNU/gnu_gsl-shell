#ifndef GUI_ELEMENT_H
#define GUI_ELEMENT_H

#include <fx.h>

#include "defs.h"

__BEGIN_DECLS
#include "lua.h"
#include "lauxlib.h"
__END_DECLS

#include "lua-cpp-utils.h"
#include "gui-types.h"

typedef gslshell::ret_status err;

struct gui_element {
  virtual int handle(lua_State* L, FXApp* app, int method_id, err& st) = 0;

  virtual FXDrawable*  as_drawable() = 0;
  virtual FXWindow*    as_window() = 0;
  virtual FXComposite* as_composite() = 0;

  virtual ~gui_element() { };
};

class gui_drawable : public gui_element {
public:
  gui_drawable(FXDrawable* w) : m_widget(w) { }

  virtual FXDrawable*  as_drawable()  { return m_widget; }
  virtual FXWindow*    as_window()    { return 0; }
  virtual FXComposite* as_composite() { return 0; }

  virtual int handle(lua_State* L, FXApp* app, int method_id, err& st) {
    switch (method_id) {
    case gui::close:
      m_widget->handle(app, FXSEL(SEL_CLOSE,0), NULL);
      break;
    case gui::enable:
      m_widget->handle(app, FXSEL(SEL_COMMAND,FXWindow::ID_ENABLE), NULL);
      break;
    case gui::disable:
      m_widget->handle(app, FXSEL(SEL_COMMAND,FXWindow::ID_DISABLE), NULL);
      break;
    case gui::check:
      m_widget->handle(app, FXSEL(SEL_COMMAND,FXWindow::ID_CHECK), NULL);
      break;
    case gui::uncheck:
      m_widget->handle(app, FXSEL(SEL_COMMAND,FXWindow::ID_UNCHECK), NULL);
      break;
    case gui::set_unknown:
      m_widget->handle(app, FXSEL(SEL_COMMAND,FXWindow::ID_UNKNOWN), NULL);
      break;
    default:
      st.error("not implemented", "gui element method");
    }

    return 0;
  }

protected:
  FXDrawable* m_widget;
};

class gui_window : public gui_drawable {
public:
  gui_window(FXWindow* w) : gui_drawable(w) { }

  virtual FXWindow*    as_window()    { return (FXWindow*) m_widget; }

  virtual int handle(lua_State* L, FXApp* app, int method_id, err& st);
};

class gui_composite : public gui_window {
public:
  gui_composite(FXComposite* obj) : gui_window(obj) { }

  virtual FXComposite* as_composite() { return (FXComposite*) m_widget; }
};

class text_field : public gui_window {
public:
  text_field(FXTextField* tf) : gui_window(tf) { }

  virtual int handle(lua_State* L, FXApp* app, int method_id, err& st);
};

class combo_box : public gui_window {
public:
  combo_box(FXComboBox* w) : gui_window(w) { }

  virtual int handle(lua_State* L, FXApp* app, int method_id, err& st);
};

#endif
