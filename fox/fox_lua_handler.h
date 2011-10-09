#ifndef FOX_LUA_HANDLER_H
#define FOX_LUA_HANDLER_H

#include <fx.h>

#include "defs.h"
#include "dict.h"
#include "gui_element.h"

__BEGIN_DECLS

#include "lua.h"
#include "lauxlib.h"

__END_DECLS

class fox_lua_handler {
public:
  fox_lua_handler() : 
    m_L(0), m_thread_id(-1), m_env_handler_index(0),
    m_current_event(0), m_current_dc(0), m_gsl_shell_locked(false),
    m_resources(0)
  { }

  ~fox_lua_handler() {
    typedef dict<int, gui_element*> obj_dict;
    for (obj_dict::iterator* p = m_objects.start(); p; p = m_objects.next(p)) {
      gui_element* obj = p->content().value;
      delete obj;
    }

    for (list<FXObject*>* p = m_resources; p; p = p->next()) {
      FXObject* obj = p->content();
      delete obj;
    }

    delete m_current_dc;
  }

  virtual long handle(FX::FXObject* sender,FX::FXSelector sel,void* ptr);

  void add_resource(FXObject* obj) {
    m_resources = new list<FXObject*>(obj, m_resources);
  }

  void bind(int id, gui_element* obj) { m_objects.insert(id, obj); }
  void map(const char* name, int id) { m_symbols.insert(name, id); }

  FXEvent* event() { return m_current_event; }

  void set_dc(FXDrawable* w);

  FXDCWindow* current_dc() { return m_current_dc; }

  void close_handler_call();

  gui_element* lookup(int id) {
    gui_element* elem;
    if (m_objects.search(id, elem)) {
      return elem;
    }
    return NULL;
  }

  bool lookup_name(const char *name, int& id) { return m_symbols.search(name, id); }

  int assign_handler(FX::FXuint sel);

  void set_lua_state(lua_State* L, int thread_id) { 
    m_L = L;
    m_thread_id = thread_id;
  }

  lua_State* lua_state()     { return m_L; }
  int        lua_thread_id() { return m_thread_id; }

private:
  dict<int, gui_element*> m_objects;
  dict<FXString, int> m_symbols;

  lua_State* m_L;
  int m_thread_id;

  dict<FX::FXuint, int> m_sel_map;
  int m_env_handler_index;

  FXEvent* m_current_event;
  FXDCWindow* m_current_dc;

  bool m_gsl_shell_locked;
  list<FXObject*>* m_resources;
};

#endif
