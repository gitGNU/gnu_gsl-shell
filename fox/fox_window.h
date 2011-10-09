#ifndef FOX_WINDOW_H
#define FOX_WINDOW_H

#include <fx.h>

#include "defs.h"
#include "fox_lua_handler.h"
#include "list.h"

__BEGIN_DECLS
#include "lua.h"
__END_DECLS

extern void window_build(lua_State* L, fox_lua_handler* hnd, FXTopWindow* win, int win_id);

class fox_window : public FXMainWindow {
public:
  fox_window(lua_State* L, FXApp* app, fox_lua_handler* hnd, const char* title,
	     int id, int opts, int w, int h) 
    : FXMainWindow(app, title, NULL, NULL, opts, 0, 0, w, h), m_handler(hnd)
  {
    window_build(L, hnd, this, id);
  }

  virtual ~fox_window() { m_handler->free_resources(); }

  virtual long handle(FX::FXObject* sender,FX::FXSelector sel,void* ptr);
  virtual void create();

private:
  fox_lua_handler* m_handler;
};

#endif
