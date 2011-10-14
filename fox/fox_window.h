#ifndef FOX_WINDOW_H
#define FOX_WINDOW_H

#include <fx.h>

#include "defs.h"
#include "fox_lua_handler.h"
#include "list.h"

__BEGIN_DECLS
#include "lua.h"
__END_DECLS

struct widget_initializer {
  int widget_id;
  int fenv_func_index;

  widget_initializer(int id, int idx) : widget_id(id), fenv_func_index(idx) {}
};

extern void window_build(lua_State* L, fox_lua_handler* hnd, FXTopWindow* win, int win_id, list<widget_initializer>*& init_ls);

class fox_window : public FXMainWindow {
public:
  fox_window(lua_State* L, FXApp* app, fox_lua_handler* hnd, const char* title,
	     int id, int opts, int w, int h, list<widget_initializer>*& init_ls) 
    : FXMainWindow(app, title, NULL, NULL, opts, 0, 0, w, h), m_handler(hnd)
  {
    window_build(L, hnd, this, id, init_ls);
  }

  virtual ~fox_window() { m_handler->free_resources(); }

  virtual long handle(FX::FXObject* sender,FX::FXSelector sel,void* ptr);
  virtual void create();

private:
  fox_lua_handler* m_handler;
};

class fox_dialog : public FXDialogBox {
public:
  fox_dialog(lua_State* L, FXApp* app, fox_lua_handler* hnd, const char* title,
	     int id, int opts, int w, int h, list<widget_initializer>*& init_ls) 
    : FXDialogBox(app, title, opts, 0, 0, w, h), m_handler(hnd)
  {
    window_build(L, hnd, this, id, init_ls);
  }

  virtual ~fox_dialog() { m_handler->free_resources(); }

  virtual long handle(FX::FXObject* sender,FX::FXSelector sel,void* ptr);

private:
  fox_lua_handler* m_handler;
};

#endif
