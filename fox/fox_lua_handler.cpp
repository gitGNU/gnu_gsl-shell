#include "fox_lua_handler.h"
#include "window_registry.h"

long fox_lua_handler::handle(FX::FXObject* sender,FX::FXSelector sel,void* ptr)
{ 
  int env_index;

  if (m_sel_map.search(sel, env_index)) {

    FXuint seltp = FXSELTYPE(sel);
    if (seltp == FX::SEL_PAINT || seltp == FX::SEL_MOTION) {
      m_current_event = (FXEvent*) ptr;
    }

    interp_lock();

    lua_State* L = m_L;
    // here we assume that we always have a lua_fox_window object
    // at the bottom of Lua stack at index position 1
    lua_getfenv(L, 1);
    lua_rawgeti(L, -1, env_index);
    lua_pushvalue(L, 1);

    int err = lua_pcall(L, 1, 0, 0);

    if (err != 0) {
      const char* msg = lua_tostring(L, -1);
      fprintf(stderr, "error in callback function: %s", msg);
      lua_pop(L, 2);
    } else {
      lua_pop(L, 1);
    }

    close_handler_call();

    interp_unlock();
      
    return (err == 0 ? 1 : 0);
  }

  return 0;
};

void fox_lua_handler::set_dc(FXDrawable* w)
{
  delete m_current_dc;
  m_current_dc = new FXDCWindow(w);
}

void fox_lua_handler::close_handler_call()
{
  m_current_event = 0;
  delete m_current_dc;
  m_current_dc = 0;
}

int fox_lua_handler::assign_handler(FX::FXuint sel)
{
  int index = (++ m_env_handler_index);
  m_sel_map.insert(sel, index);
  return index;
}
