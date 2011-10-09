#ifndef LUA_FOX_WINDOW_H
#define LUA_FOX_WINDOW_H

#include <fx.h>

#include "defs.h"
#include "fox_window.h"
#include "fox_lua_handler.h"
#include "gui_element.h"

__BEGIN_DECLS

#include "lua.h"
#include "lauxlib.h"

extern void fox_window_register (lua_State *L);

__END_DECLS

class lua_fox_window {
public:
  enum win_status_e { not_started, starting, running, error, closed };

  lua_fox_window() : status(not_started), m_window(0) {}

  ~lua_fox_window();

  void init(fox_window* win) {
    m_window = win;
  }

  FXApp*           app()         { return m_window->getApp(); }
  fox_window*      window()      { return m_window; }
  fox_lua_handler* lua_handler() { return &m_handler; }

  int protected_call(lua_State* L, int (lua_fox_window::*method)(lua_State*, err&));

  int handle_msg (lua_State* L, err& st);
  int dc_handle  (lua_State* L, err& st);

  enum win_status_e status;

private:
  fox_window* m_window;
  fox_lua_handler m_handler;
};

namespace dc_operation {
  enum dc_oper_e {
    set_foreground = 1,
    draw_line      = 2,
    fill_rectangle = 3,
  };
};

#endif
