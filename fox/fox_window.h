#ifndef FOX_WINDOW_H
#define FOX_WINDOW_H

#include <fx.h>

#include "defs.h"
#include "fox_app.h"

__BEGIN_DECLS

#include "lua.h"
#include "lauxlib.h"

extern void fox_window_register (lua_State *L);

__END_DECLS

class fox_window : public FXMainWindow {
public:
  fox_window(lua_State* L, fox_app* app, const char* title, int id, int opts, int w, int h);
};

class lua_fox_window {
public:
  enum win_status_e { not_ready, starting, running, error, closed };

  lua_fox_window() : status(not_ready), m_app(0), m_window(0) {}

  ~lua_fox_window();

  void init(fox_app* app, fox_window* win) {
    m_app = app;
    m_window = win;
  }

  fox_app*    app()    { return m_app; }
  fox_window* window() { return m_window; }

  enum win_status_e status;

private:
  fox_app* m_app;
  fox_window* m_window;
};

#endif
