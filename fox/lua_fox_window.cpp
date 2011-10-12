#include <new>
#include <assert.h>

#include "lua_fox_window.h"
#include "fox_window.h"
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
static int fox_window_dialog_create(lua_State* L);
static int fox_window_execute(lua_State* L);
static int fox_window_yield(lua_State* L);

static const struct luaL_Reg fox_window_functions[] = {
  {"fox_window", fox_window_new},
  {NULL, NULL}
};

static const struct luaL_Reg fox_window_methods[] = {
  {"run",       fox_window_run},
  {"element",   fox_window_get_element},
  {"dialog",    fox_window_dialog_create},
  {"execute",   fox_window_execute},
  {"yield",     fox_window_yield},
  {"handle",    fox_window_handle_msg},
  {"self",      fox_window_self},
  {"event",     fox_window_event},
  {"getDC",     fox_window_get_dc},
  {"draw",      fox_window_dc_handle},
  {"__gc",      fox_window_free},
  {NULL, NULL}
};

__END_DECLS

enum {
  FOX_MAIN_WINDOW_INDEX = (1 << 16),
  FOX_RET_VALUES_INDEX,
};

lua_fox_window::~lua_fox_window()
{
  if (status == not_started || status == error) {
    if (m_is_main)
      delete this->app();
    else
      delete this->m_window;
  }
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

    new((void*) lwin) lua_fox_window(fox_window::ID_LAST, true);

    lua_newtable(L);
    lua_getfield(L, -3, "body");

    FXApp* app = new FXApp("GSL Shell", "Francesco Abbate");
    fox_window* win = new fox_window(L, app, lwin->lua_handler(), title, id, opts, width, height);

    lwin->init(win);

    lua_pop(L, 1);
    lua_setfenv(L, -2);

    return 1;
  }

  return luaL_error (L, "invalid contructor type_id: %i", type_id);
}

int fox_window_dialog_create(lua_State* L)
{
  lua_fox_window* lmainwin = object_check<lua_fox_window>(L, 1, GS_FOX_WINDOW);

  int type_id = get_int_field(L, "type_id");
  int id      = get_int_field(L, "id");

  if (type_id == gui::dialog_box) {
    lua_getfield(L, -1, "args");

    lua_rawgeti(L, -1, 1);
    const char* title = lua_tostring(L, -1);
    lua_pop(L, 1);

    int opts   = get_int_element(L, 2);
    int width  = get_int_element(L, 3);
    int height = get_int_element(L, 4);
      
    lua_pop(L, 1);

    lua_fox_window* lwin = (lua_fox_window*) gs_new_object(sizeof(lua_fox_window), L, GS_FOX_WINDOW);

    new((void*) lwin) lua_fox_window(fox_dialog::ID_LAST, false);

    lwin->lua_handler()->set_locker(lmainwin->lua_handler()->locker());

    lua_newtable(L);
    lua_getfield(L, -3, "body");

    fox_dialog* win = new fox_dialog(L, lmainwin->app(), lwin->lua_handler(), title, id, opts, width, height);

    lwin->init(win);

    lua_pop(L, 1);

    lua_pushvalue(L, 1);
    lua_rawseti(L, -2, FOX_MAIN_WINDOW_INDEX);

    lua_setfenv(L, -2);

    return 1;
  }

  return luaL_error (L, "invalid contructor type_id: %i", type_id);
}

static int fox_window_run_modal(lua_State* L)
{
  lua_fox_window* lwin = object_check<lua_fox_window>(L, 1, GS_FOX_WINDOW);
  FXTopWindow* win = lwin->window();
  FXApp* app = lwin->app();

  lua_getfenv(L, 1);
  lua_rawgeti(L, -1, FOX_MAIN_WINDOW_INDEX);

  lua_insert(L, 1);
  lua_settop(L, 1);

  lwin->status = lua_fox_window::running;

  win->create();
  win->show(PLACEMENT_CURSOR);
  app->refresh();
  app->runModalFor(win);

  return 0;
}

/* retrieve the FENV associated to the main window of the window located
   at "index" */
static void get_main_window_fenv(lua_State* L, int index)
{
  lua_getfenv(L, index);
  lua_rawgeti(L, -1, FOX_MAIN_WINDOW_INDEX);
  lua_getfenv(L, -1);
  lua_insert(L, -3);
  lua_pop(L, 2);
}

int fox_window_execute(lua_State* L)
{
  lua_fox_window* lwin = object_check<lua_fox_window>(L, 1, GS_FOX_WINDOW);

  lua_State* new_L = lua_newthread(L);

  lwin->lua_handler()->set_lua_state(new_L);

  lua_pushvalue(L, 1);
  lua_xmove(L, new_L, 1);

  lua_pushcfunction(L, fox_window_run_modal);
  lua_pushvalue(L, 1);
  lua_call(L, 1, 0);

  get_main_window_fenv(L, 1);

  lua_rawgeti(L, -1, FOX_RET_VALUES_INDEX);
  int n = lua_tointeger(L, -1);
  lua_pop(L, 1);

  for (int k = 1; k <= n; k++) {
    lua_rawgeti(L, -k, FOX_RET_VALUES_INDEX + k);
  }

  return n;
}

int fox_window_yield(lua_State* L)
{
  lua_fox_window* lwin = object_check<lua_fox_window>(L, 1, GS_FOX_WINDOW);
  FXTopWindow* win = lwin->window();
  FXApp* app = lwin->app();
  int n = lua_gettop(L) - 1;
  
  get_main_window_fenv(L, 1);

  lua_pushinteger(L, n);
  lua_rawseti(L, -2, FOX_RET_VALUES_INDEX);

  lua_insert(L, 2);

  for (int k = n; k >= 1; k--) {
    lua_rawseti(L, 2, FOX_RET_VALUES_INDEX + k);
  }

  lua_pop(L, 1);

  app->stopModal(win);
  win->hide();

  lwin->dispose_window();

  return n;
}

int fox_window_free(lua_State* L)
{
  lua_fox_window* lwin = object_check<lua_fox_window>(L, 1, GS_FOX_WINDOW);
  lwin->~lua_fox_window();
  return 0;
}

void thread_interp_locker::lock()
{
  if (m_nest == 0)
    GSL_SHELL_LOCK();
  m_nest ++;
}

void thread_interp_locker::unlock()
{
  m_nest --;
  if (m_nest == 0)
    GSL_SHELL_UNLOCK();
}

static void *
fox_window_thread_function (void *_inf)
{
  lua_fox_window* lwin = (lua_fox_window*) _inf;
  fox_lua_handler* lua_handler = lwin->lua_handler();
  FXApp* app = lwin->app();
  thread_interp_locker locker;

  lua_handler->set_locker(&locker);

  lua_State* L = lua_handler->lua_state();

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
  window_index_remove (L, lua_handler->lua_thread_id());
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

  lwin->status = lua_fox_window::starting;

  lua_State* new_L = lua_newthread(L);
  int thread_id = window_index_add(L, -1);
  lua_pop(L, 1);

  lwin->lua_handler()->set_lua_state(new_L, thread_id);

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
  if (lwin->lua_handler()->lookup_name(key, id)) {
    lua_pushinteger(L, id);
    return 1;
  }

  return 0;
}

int
lua_fox_window::protected_call(lua_State* L,
			       int (lua_fox_window::*method)(lua_State*, err&))
{
  bool is_fox_thread = (L == lua_handler()->lua_state());
  err status;
  int retval = 0;

  if (!is_fox_thread) {
    app()->mutex().lock();
  }

  if (this->status == lua_fox_window::running) {
    retval = (this->*method)(L, status);
  } else {
    status.error("window is not running", "window method");
  }

  if (!is_fox_thread) {
    app()->flush();
    app()->mutex().unlock();
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

  gui_element* elem = lua_handler()->lookup(widget_id);

  if (!elem) {
    st.error("invalid element id", "FOX window handle");
    return 0;
  }

  return elem->handle(L, app(), method_id, st);
}

int fox_window_handle_msg(lua_State* L)
{
  lua_fox_window* lwin = object_check<lua_fox_window>(L, 1, GS_FOX_WINDOW);
  return lwin->protected_call(L, &lua_fox_window::handle_msg);
}


int fox_window_event(lua_State* L)
{
  lua_fox_window* lwin = object_check<lua_fox_window>(L, 1, GS_FOX_WINDOW);
  FXEvent* event = lwin->lua_handler()->event();

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
  fox_lua_handler* hnd = lwin->lua_handler();
  int id = luaL_checkinteger(L, 2);
  FXDrawable* draw = hnd->lookup(id)->as_drawable();
  if (!draw)
    luaL_error(L, "cannor associate a DC");
  hnd->set_dc(draw);
  return 0;
}

int
lua_fox_window::dc_handle(lua_State* L, err& st)
{
  int oper_id = lua_tointeger(L, 2);
  
  FXDCWindow* dc = lua_handler()->current_dc();

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
  lwin->lua_handler()->lookup_name("*", id);
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
