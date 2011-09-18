#include "fox_app.h"
#include "gsl-shell.h"
#include "window_registry.h"

const FX::FXMetaClass fox_app::metaClass("fox_app", fox_app::manufacture, &FXApp::metaClass, NULL, 0, 0);

long fox_app::handle(FX::FXObject* sender,FX::FXSelector sel,void* ptr)
{ 
  int env_index;

  if (m_sel_map.search(sel, env_index)) {
    GSL_SHELL_LOCK();
    lua_State* L = m_L;
    window_index_get(L, m_window_id);
    lua_getfenv(L, -1);
    lua_rawgeti(L, -1, env_index);
    lua_pushvalue(L, 1);
    lua_call(L, 1, 1);
    int status = lua_tointeger(L, -1);
    lua_pop(L, 3);
    GSL_SHELL_UNLOCK();
    return status;
  }

  return FXApp::handle(sender, sel, ptr);
};

int fox_app::assign_handler(FX::FXuint sel)
{
  int index = (++ m_env_handler_index);
  m_sel_map.insert(sel, index);
  return index;
}
