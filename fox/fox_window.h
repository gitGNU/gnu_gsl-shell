#ifndef FOX_WINDOW_H
#define FOX_WINDOW_H

#include <fx.h>

#include "defs.h"
#include "fox_lua_handler.h"
#include "list.h"

__BEGIN_DECLS
#include "lua.h"
__END_DECLS

class fox_window : public FXMainWindow {
public:
  fox_window(lua_State* L, FXApp* app, const char* title,
	     int id, int opts, int w, int h) 
    : FXMainWindow(app, title, NULL, NULL, opts, 0, 0, w, h), m_resources(0)
  {
    window_build(L, m_handler, this, id);
  }

  virtual ~fox_window() {
    for (list<FXObject*>* p = m_resources; p; p = p->next()) {
      FXObject* obj = p->content();
      delete obj;
    }
  }

  virtual long handle(FX::FXObject* sender,FX::FXSelector sel,void* ptr);
  virtual void create();

  fox_lua_handler* lua_handler() { return &m_handler; }

  void add_resource(FXObject* obj) {
    m_resources = new list<FXObject*>(obj, m_resources);
  }

protected:
  static void window_build(lua_State* L, fox_lua_handler& hnd, fox_window* win, int win_id);

private:
  list<FXObject*>* m_resources;
  fox_lua_handler m_handler;
};

#endif
