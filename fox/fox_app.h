#ifndef FOX_APP_H
#define FOX_APP_H

#include <fx.h>

#include "defs.h"
#include "dict.h"
#include "gui_element.h"

__BEGIN_DECLS

#include "lua.h"
#include "lauxlib.h"

__END_DECLS

class fox_app : public FXApp {
public:
  static const FX::FXMetaClass metaClass;

  static FX::FXObject* manufacture() { return new fox_app; }

  virtual long handle(FX::FXObject* sender,FX::FXSelector sel,void* ptr);

  virtual const FX::FXMetaClass* getMetaClass() const { return &metaClass; }

  friend FX::FXStream& operator<<(FX::FXStream& store, const fox_app* obj) {
    return store.saveObject((FX::FXObjectPtr)(obj)); 
  }

  friend FX::FXStream& operator>>(FX::FXStream& store, fox_app*& obj) {
    return store.loadObject((FX::FXObjectPtr&)(obj));
  }

  fox_app() : FXApp("FOX App test"),
	      m_L(0), m_thread_id(-1), m_env_handler_index(0),
	      m_current_event(0), m_current_dc(0)
  { }

  ~fox_app() {
    typedef dict<int, gui_element*> obj_dict;
    for (obj_dict::iterator* p = m_objects.start(); p; p = m_objects.next(p)) {
      gui_element* obj = p->content().value;
      delete obj;
    }
    delete m_current_dc;
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
  fox_app(const fox_app&);
  fox_app &operator=(const fox_app&);

  dict<int, gui_element*> m_objects;
  dict<FXString, int> m_symbols;

  lua_State* m_L;
  int m_thread_id;

  dict<FX::FXuint, int> m_sel_map;
  int m_env_handler_index;

  FXEvent* m_current_event;
  FXDCWindow* m_current_dc;
};

#endif
