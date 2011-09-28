#include "fox_app.h"
#include "gsl-shell.h"
#include "window_registry.h"

const FX::FXMetaClass fox_app::metaClass("fox_app", fox_app::manufacture, &FXApp::metaClass, NULL, 0, 0);

long fox_app::handle(FX::FXObject* sender,FX::FXSelector sel,void* ptr)
{ 
  int env_index;

  if (m_sel_map.search(sel, env_index)) {

    FXuint seltp = FXSELTYPE(sel);
    if (seltp == FX::SEL_PAINT || seltp == FX::SEL_MOTION) {
      m_current_event = (FXEvent*) ptr;
    }

    GSL_SHELL_LOCK();

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

    GSL_SHELL_UNLOCK();
    return (err == 0 ? 1 : 0);
  }

  return FXApp::handle(sender, sel, ptr);
};

void fox_app::set_dc(FXDrawable* w)
{
  delete m_current_dc;
  m_current_dc = new FXDCWindow(w);
}

void fox_app::close_handler_call()
{
  m_current_event = 0;
  delete m_current_dc;
  m_current_dc = 0;
}

int fox_app::assign_handler(FX::FXuint sel)
{
  int index = (++ m_env_handler_index);
  m_sel_map.insert(sel, index);
  return index;
}
