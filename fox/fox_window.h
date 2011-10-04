#ifndef FOX_WINDOW_H
#define FOX_WINDOW_H

#include <fx.h>

#include "defs.h"
#include "fox_app.h"

__BEGIN_DECLS
#include "lua.h"
__END_DECLS

class fox_window : public FXMainWindow {
public:
  fox_window(lua_State* L, fox_app* app, const char* title, int id, int opts, int w, int h);

  virtual void create();
};

#endif
