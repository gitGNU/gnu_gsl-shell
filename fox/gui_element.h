#ifndef GUI_ELEMENT_H
#define GUI_ELEMENT_H

#include <fx.h>

#include "defs.h"

__BEGIN_DECLS
#include "lua.h"
#include "lauxlib.h"
__END_DECLS

#include "lua-cpp-utils.h"

template <typename Fox_object>
struct gui_element_gen {
  virtual int handle(lua_State* L, gslshell::ret_status& st) = 0;
  virtual Fox_object* content() = 0;
  virtual ~gui_element_gen() { };
};

typedef gui_element_gen<FX::FXDrawable> gui_element;

template <typename Fox_widget>
class fox_gui_element : public gui_element {
public:
  fox_gui_element(Fox_widget* w) : m_widget(w) { }

  Fox_widget* operator->()      { return m_widget; }
  virtual FXDrawable* content() { return m_widget; };

  virtual int handle(lua_State* L, gslshell::ret_status& st) {
    return not_implemented(st); 
  }

private:
  int not_implemented(gslshell::ret_status& st) {
    st.error("not implemented", "gui element method");
    return 0;
  }

protected:
  Fox_widget* m_widget;
};

class text_field : public fox_gui_element<FXTextField> {
public:
  text_field(FXTextField* tf) : fox_gui_element<FXTextField>(tf) { }

  virtual int handle(lua_State* L, gslshell::ret_status& st);
};

class gui_window : public fox_gui_element<FXMainWindow> {
public:
  gui_window(FXMainWindow* tf) : fox_gui_element<FXMainWindow>(tf) { }

  virtual int handle(lua_State* L, gslshell::ret_status& st);
};

#endif
