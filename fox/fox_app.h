#ifndef FOX_APP_H
#define FOX_APP_H

#include <fx.h>

#include "defs.h"
#include "dict.h"

// __BEGIN_DECLS

// #include "lua.h"
// #include "lauxlib.h"

// __END_DECLS

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

public:
  fox_app() : FXApp("FOX App test") { }

  void bind(int id, FXObject* obj) { m_objects.insert(id, obj); }
  void map(const char* name, int id) { m_symbols.insert(name, id); }

  FXObject* get_object_by_id(int id) {
    FXObject* obj;
    if (m_objects.search(id, obj))
      return obj;
    return NULL;
  }

private:
  fox_app(const fox_app&);
  fox_app &operator=(const fox_app&);

  dict<int, FXObject*> m_objects;
  dict<FXString, int> m_symbols;
};

#endif
