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

struct lua_fox_window {
  enum win_status_e { not_ready, starting, running, error, closed };

  lua_fox_window() : status(not_ready), window(0) {}

  enum win_status_e status;
  fox_window* window;
};

#endif
