#ifndef FOX_LUA_HANDLER_H
#define FOX_LUA_HANDLER_H

#include <memory>

#include <fx.h>

#include "defs.h"
#include "dict.h"
#include "gui_element.h"
#include "gsl-shell.h"

__BEGIN_DECLS

#include "lua.h"
#include "lauxlib.h"

__END_DECLS


class thread_interp_locker {
public:
  thread_interp_locker() : m_nest(0) {}

  void lock() {
    if (m_nest == 0)
      GSL_SHELL_LOCK();
    m_nest ++;
  }

  void unlock() {
    m_nest --;
    if (m_nest == 0)
      GSL_SHELL_UNLOCK();
  }

private:
  int m_nest;
};

class fox_lua_handler {
  typedef std::auto_ptr<FXObject> auto_obj;
public:
  fox_lua_handler(FXuint id_last) : 
  m_L(0), m_thread_id(-1), m_window_index(-1),
    m_env_handler_index(0), m_current_event(0), m_current_dc(0),
    m_resources(0), m_nb_retval(0), m_id_last(id_last), m_interp_lock(0)
  { }

  ~fox_lua_handler() {
    typedef dict<int, gui_element*> obj_dict;
    for (obj_dict::iterator* p = m_objects.start(); p; p = m_objects.next(p)) {
      gui_element* obj = p->content().value;
      delete obj;
    }

    delete m_current_dc;

    free_resources();
  }

  void free_resources() {
    list<auto_obj>::free(m_resources);
    m_resources = 0;
  }

  virtual long handle(FX::FXObject* sender,FX::FXSelector sel,void* ptr);

  void add_resource(FXObject* obj) {
    auto_obj ref(obj);
    m_resources = new list<auto_obj>(ref, m_resources);
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

  int get_handler_slot() { return (++ m_env_handler_index); }
  int assign_handler(FX::FXuint sel);

  void set_lua_state(lua_State* L, int window_index, int thread_id = -1) { 
    m_L = L;
    m_window_index = window_index;
    m_thread_id = thread_id;
  }

  lua_State* lua_state()     { return m_L; }
  int        lua_thread_id() { return m_thread_id; }

  int  nb_return_values() const { return m_nb_retval; }
  void add_return_values(int n) { m_nb_retval = n; }

  FXuint id_last() const { return m_id_last; }

  thread_interp_locker* locker() { return m_interp_lock; }
  void set_locker(thread_interp_locker* locker) { m_interp_lock = locker; }

  void interp_lock()   const { m_interp_lock->lock();   }
  void interp_unlock() const { m_interp_lock->unlock(); }

private:
  void inherit(const fox_lua_handler* parent) {
    m_L = parent->m_L;
    m_thread_id = parent->m_thread_id;
    m_interp_lock = parent->m_interp_lock;
  }

  dict<int, gui_element*> m_objects;
  dict<FXString, int> m_symbols;

  lua_State* m_L;
  int m_thread_id;
  int m_window_index;

  dict<FX::FXuint, int> m_sel_map;
  int m_env_handler_index;

  FXEvent* m_current_event;
  FXDCWindow* m_current_dc;

  list<auto_obj>* m_resources;

  int m_nb_retval;
  FXuint m_id_last;

  thread_interp_locker* m_interp_lock;
};

#endif
