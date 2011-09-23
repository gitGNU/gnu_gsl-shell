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
  virtual int lua_call(lua_State* L, gslshell::ret_status& st) = 0;
  virtual Fox_object* content() = 0;
  virtual ~gui_element_gen() { };
};

typedef gui_element_gen<FX::FXObject> gui_element;

class text_field : public gui_element {
public:
  text_field(FXTextField* tf) : m_text_field(tf) { }

  FXTextField* operator->() { return m_text_field; }

  virtual int lua_call(lua_State* L, gslshell::ret_status& st);
  virtual FXObject* content() { return m_text_field; };

private:
  FXTextField* m_text_field;
};


template <typename Fox_element>
class fake_element : public gui_element {
public:
  fake_element(Fox_element* obj) : m_object(obj) { }

  Fox_element* operator->() { return m_object; }

  virtual int lua_call(lua_State* L, gslshell::ret_status& st) {
    st.error("not implemented", "gui element method");
    return 0;
  }

  virtual FXObject* content() { return m_object; };

private:
  Fox_element* m_object;
};

#endif
