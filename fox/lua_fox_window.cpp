#include <new>
#include <assert.h>

#include "lua_fox_window.h"
#include "gui-types.h"
#include "gs-types.h"
#include "lua-cpp-utils.h"
#include "gsl-shell.h"
#include "window_registry.h"
#include "gui_element.h"
#include "unpack.h"

__BEGIN_DECLS

static int fox_window_run(lua_State* L);
static int fox_window_new(lua_State* L);
static int fox_window_free(lua_State* L);
static int fox_window_get_element(lua_State* L);
static int fox_window_handle_msg(lua_State* L);
static int fox_window_event(lua_State* L);
static int fox_window_get_dc(lua_State* L);
static int fox_window_dc_handle(lua_State* L);
static int fox_window_self(lua_State* L);

static const struct luaL_Reg fox_window_functions[] = {
  {"fox_window", fox_window_new},
  {NULL, NULL}
};

static const struct luaL_Reg fox_window_methods[] = {
  {"run",       fox_window_run},
  {"element",   fox_window_get_element},
  {"handle",    fox_window_handle_msg},
  {"self",      fox_window_self},
  {"event",     fox_window_event},
  {"getDC",     fox_window_get_dc},
  {"draw",      fox_window_dc_handle},
  {"__gc",      fox_window_free},
  {NULL, NULL}
};

__END_DECLS

typedef lua_thread_info<lua_fox_window> thread_info;

lua_fox_window::~lua_fox_window()
{
  if (status == not_started || status == error)
    delete m_app;
}

int fox_window_new(lua_State* L)
{
  int type_id = get_int_field(L, "type_id");
  int id      = get_int_field(L, "id");

  if (type_id == gui::main_window) {
    lua_getfield(L, -1, "args");

    lua_rawgeti(L, -1, 1);
    const char* title = lua_tostring(L, -1);
    lua_pop(L, 1);

    int opts   = get_int_element(L, 2);
    int width  = get_int_element(L, 3);
    int height = get_int_element(L, 4);
      
    lua_pop(L, 1);

    lua_fox_window* lwin = (lua_fox_window*) gs_new_object(sizeof(lua_fox_window), L, GS_FOX_WINDOW);

    lua_newtable(L);
    lua_getfield(L, -3, "body");

    fox_app* app = new fox_app();
    fox_window* win = new fox_window(L, app, title, id, opts, width, height);

    lwin->init(app, win);

    lua_pop(L, 1);
    lua_setfenv(L, -2);

    return 1;
  }

  return luaL_error (L, "invalid contructor type_id: %i", type_id);
}

int fox_window_free(lua_State* L)
{
  lua_fox_window* lwin = object_check<lua_fox_window>(L, 1, GS_FOX_WINDOW);
  lwin->~lua_fox_window();
  return 0;
}

static void *
fox_window_thread_function (void *_inf)
{
  lua_fox_window* lwin = (lua_fox_window*) _inf;
  fox_app* app = lwin->app();

  lua_State* L = app->lua_state();

  int argc = 1;
  char null[1] = {'\0'};
  char *argv[1];

  argv[0] = null;

  // Open display
  app->init(argc, argv);

  lwin->status = lua_fox_window::running;

  app->create();
  app->run();

  GSL_SHELL_LOCK();
  window_index_remove (L, app->lua_thread_id());
  GSL_SHELL_UNLOCK();

  lwin->status = lua_fox_window::closed;
  delete app;

  return NULL;
}

int fox_window_run(lua_State* L)
{
  lua_fox_window* lwin = object_check<lua_fox_window>(L, 1, GS_FOX_WINDOW);

  if (lwin->status != lua_fox_window::not_started)
    return luaL_error(L, "window already running or terminated");

  fox_app* app = lwin->app();

  lwin->status = lua_fox_window::starting;

  lua_State* new_L = lua_newthread(L);
  int thread_id = window_index_add(L, -1);
  lua_pop(L, 1);

  app->set_lua_state(new_L, thread_id);

  lua_settop(L, 1);
  lua_xmove(L, new_L, 1);

  pthread_attr_t attr[1];
  pthread_t win_thread[1];

  pthread_attr_init (attr);
  pthread_attr_setdetachstate (attr, PTHREAD_CREATE_DETACHED);

  void *user_data = (void *) lwin;
  if (pthread_create(win_thread, attr, fox_window_thread_function, user_data))
    {
      lwin->status = lua_fox_window::error; 
      pthread_attr_destroy (attr);
      window_index_remove (L, thread_id);
      return luaL_error(L, "error during thread initialization");
    }
  else
    {
      pthread_attr_destroy (attr);
    }

  return 0;
}

int fox_window_get_element(lua_State* L)
{
  lua_fox_window* lwin = object_check<lua_fox_window>(L, 1, GS_FOX_WINDOW);
  const char* key = lua_tostring(L, 2);

  int id;
  if (lwin->app()->lookup_name(key, id)) {
    lua_pushinteger(L, id);
    return 1;
  }

  return 0;
}

int
lua_fox_window::protected_call(lua_State* L,
			       int (lua_fox_window::*method)(lua_State*, err&))
{
  bool is_fox_thread = (L == m_app->lua_state());
  err status;
  int retval = 0;

  if (!is_fox_thread) {
    m_app->mutex().lock();
  }

  if (this->status == lua_fox_window::running) {
    retval = (this->*method)(L, status);
  } else {
    status.error("window is not running", "window method");
  }

  if (!is_fox_thread) {
    m_app->flush();
    m_app->mutex().unlock();
  }

  if (status.error_msg())
    return luaL_error(L, "error %s in %s.", status.error_msg(), status.context());

  return retval;
}

int
lua_fox_window::handle_msg(lua_State* L, err& st)
{
  int widget_id = luaL_checkinteger(L, 2);
  int method_id = luaL_checkinteger(L, 3);

  gui_element* elem = m_app->lookup(widget_id);

  if (!elem) {
    st.error("invalid element id", "FOX window handle");
    return 0;
  }

  return elem->handle(L, m_app, method_id, st);
}

int fox_window_handle_msg(lua_State* L)
{
  lua_fox_window* lwin = object_check<lua_fox_window>(L, 1, GS_FOX_WINDOW);
  return lwin->protected_call(L, &lua_fox_window::handle_msg);
}


int fox_window_event(lua_State* L)
{
  lua_fox_window* lwin = object_check<lua_fox_window>(L, 1, GS_FOX_WINDOW);
  FXEvent* event = lwin->app()->event();

  if (!event) {
    return luaL_error(L, "no event available");
  }

  int nargs = lua_gettop(L);
  int nrv = 0;
  for (int idx = 2; idx <= nargs; idx++, nrv++) {
    int event_index = lua_tointeger(L, idx);
    bool is_unsigned = false;
    void *ptr = 0;

    if (event_index <= 2) {
      ptr = &event->type + (event_index - 1);
      is_unsigned = true; 
    } else if (event_index <= 8) {
      ptr = &event->type + (event_index - 1);
    } else if (event_index == 9) {
      lua_pushstring(L, event->text.text());
    } else if (event_index <= 15) {
      ptr = &event->last_x + (event_index - 10);
    } else if (event_index <= 17) {
      ptr = &event->last_x + (event_index - 10);
      is_unsigned = true; 
    } else if (event_index <= 18) {
      ptr = &event->last_x + (event_index - 10);
    } else if (event_index == 19) {
      lua_pushinteger(L, (int) event->moved);
    } else if (event_index == 20) {
      FXRectangle& r = event->rect;
      lua_pushinteger(L, (int) r.x);
      lua_pushinteger(L, (int) r.y);
      lua_pushinteger(L, (int) r.w);
      lua_pushinteger(L, (int) r.h);
      nrv += 3;
    } else {
      luaL_error(L, "unrecognized event id: %i", event_index);
    }

    if (ptr) {
      if (is_unsigned) {
	lua_pushnumber(L, *(FXuint *) ptr);
      } else {
	lua_pushnumber(L, *(FXint *) ptr);
      }
    }
  }

  return nrv;
}

int fox_window_get_dc(lua_State* L)
{
  lua_fox_window* lwin = object_check<lua_fox_window>(L, 1, GS_FOX_WINDOW);
  fox_app* app = lwin->app();
  int id = luaL_checkinteger(L, 2);
  FXDrawable* draw = app->lookup(id)->as_drawable();
  if (!draw)
    luaL_error(L, "cannor associate a DC");
  app->set_dc(draw);
  return 0;
}

int
lua_fox_window::dc_handle(lua_State* L, err& st)
{
  int oper_id = lua_tointeger(L, 2);
  
  FXDCWindow* dc = m_app->current_dc();

  if (!dc) {
    st.error("invalid drawing context", "drawing method");
    return 0;
  }

  switch (oper_id) {
  case dc_operation::set_foreground:
    {
      FXuint color = lua_tointeger(L, 3);
      dc->setForeground(color);
      break;
    }
  case dc_operation::draw_line:
  case dc_operation::fill_rectangle:
    {
      int x1 = lua_tointeger(L, 3);
      int y1 = lua_tointeger(L, 4);
      int x2 = lua_tointeger(L, 5);
      int y2 = lua_tointeger(L, 6);

      if (oper_id == dc_operation::draw_line)
	dc->drawLine(x1, y1, x2, y2);
      else
	dc->fillRectangle(x1, y1, x2, y2);

      break;
    }
  default:
    st.error("invalid drawing operation id", "drawing method");
  }

  return 0;
}

int fox_window_dc_handle(lua_State* L)
{
  lua_fox_window* lwin = object_check<lua_fox_window>(L, 1, GS_FOX_WINDOW);
  return lwin->protected_call(L, &lua_fox_window::dc_handle);
}

int fox_window_self(lua_State* L)
{
  lua_fox_window* lwin = object_check<lua_fox_window>(L, 1, GS_FOX_WINDOW);
  int id;
  lwin->app()->lookup_name("*", id);
  lua_pushinteger(L, id);
  return 1;
}

void
fox_window_register (lua_State *L)
{
  luaL_newmetatable (L, GS_METATABLE(GS_FOX_WINDOW));
  lua_pushvalue (L, -1);
  lua_setfield (L, -2, "__index");
  luaL_register (L, NULL, fox_window_methods);
  lua_pop (L, 1);

  luaL_register (L, NULL, fox_window_functions);
}
