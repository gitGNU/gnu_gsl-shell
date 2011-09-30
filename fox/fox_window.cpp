
#include <new>
#include <assert.h>

#include "fox_window.h"
#include "gui-types.h"
#include "gs-types.h"
#include "lua-cpp-utils.h"
#include "gsl-shell.h"
#include "window_registry.h"
#include "gui_element.h"

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

int get_int_field (lua_State* L, const char* key)
{
  lua_getfield(L, -1, key);
  int result = lua_tointeger(L, -1);
  lua_pop(L, 1);
  return result;
}

const char* get_string_field (lua_State* L, const char* key)
{
  lua_getfield(L, -1, key);
  const char* result = lua_tostring(L, -1);
  lua_pop(L, 1);
  return result;
}

int get_int_element (lua_State* L, int index)
{
  lua_rawgeti(L, -1, index);
  int result = lua_tointeger(L, -1);
  lua_pop(L, 1);
  return result;
}

const char* get_string_element (lua_State* L, int index)
{
  lua_rawgeti(L, -1, index);
  const char* result = lua_tostring(L, -1);
  lua_pop(L, 1);
  return result;
}

// unpack the table on top of the stack to assign the handlers:
// SEL => (envinronment index)
int parse_handlers(lua_State* L, int env_table_index, fox_app* app, int id)
{
  if (!lua_istable(L, -1))
    return (-1);

  FX::FXuint fox_id = fox_app::ID_LAST + id;

  int hn = lua_objlen(L, -1);
  for (int k = 1; k <= hn; k++) {
    lua_rawgeti(L, -1, k);

    lua_rawgeti(L, -1, 1);
    FX::FXuint sel = lua_tointeger(L, -1);
    lua_pop(L, 1);

    printf("Assigning handler #%i, selector: (%u, %u)\n", k, sel, id);

    lua_rawgeti(L, -1, 2);
    int env_index = app->assign_handler(FXSEL(sel,fox_id));
    lua_rawseti(L, env_table_index, env_index);

    lua_pop(L, 1);
  }

  return (int) fox_id;
}

fox_window::fox_window(lua_State* L, fox_app* app, const char* title, int win_id, int opts, int w, int h)
  : FXMainWindow(app, title, NULL, NULL, opts, 0, 0, w, h)
{
  /* position in the Lua stack of the environment table for the window */
  const int env_table_index = 3;

  app->bind(win_id, new gui_main_window(this));
  app->map("*", win_id);

  int n = lua_objlen(L, -1);

  for (int j = 1; j <= n; j++) {
    lua_rawgeti(L, -1, j);

    int type_id = get_int_field(L, "type_id");
    int id = get_int_field(L, "id");
    const char* name = get_string_field(L, "name");

    int parent_id = get_int_field(L, "parent");
    FXComposite* parent = (FXComposite*) app->get_object_by_id(parent_id);

    assert(parent);

    gui_element* elem;

    switch (type_id) {
    case gui::horizontal_frame: 
      {
	lua_getfield(L, -1, "args");
	int opts = get_int_element(L, 1);
	lua_pop(L, 1);
	FXHorizontalFrame* hf = new FXHorizontalFrame(parent, opts);
	elem = new fox_gui_element<FXHorizontalFrame>(hf);
	printf("Adding horizontal frame (id=%i) to object id= %i\n", id, parent_id);
	break;
      }
    case gui::vertical_frame: 
      {
	lua_getfield(L, -1, "args");
	int opts = get_int_element(L, 1);
	lua_pop(L, 1);
	FXVerticalFrame* hf = new FXVerticalFrame(parent, opts);
	elem = new fox_gui_element<FXVerticalFrame>(hf);
	printf("Adding vertical frame (id=%i) to object id= %i\n", id, parent_id);
	break;
      }
    case gui::button:
      {
	lua_getfield(L, -1, "args");
	const char* text = get_string_element(L, 1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "handlers");
 	int hid = parse_handlers(L, env_table_index, app, id);
	lua_pop(L, 1);

	FXButton* b = new FXButton(parent, text, NULL, hid >= 0 ? app : 0, hid);

	elem = new fox_gui_element<FXButton>(b);
	printf("Adding button (id=%i) to object id= %i\n", id, parent_id);
	break;
      }
    case gui::text_field:
      {
	lua_getfield(L, -1, "args");
	int columns = get_int_element(L, 1);
	lua_pop(L, 1);
	FXTextField* tf = new FXTextField(parent, columns);
	elem = new text_field(tf);
	printf("Adding text field (id=%i) to object id= %i\n", id, parent_id);
	break;
      }
    case gui::label:
      {
	lua_getfield(L, -1, "args");
	const char *text = get_string_element(L, 1);
	lua_pop(L, 1);
	FXLabel* label = new FXLabel(parent, text);
	elem = new fox_gui_element<FXLabel>(label);
	break;
      }
    case gui::canvas:
      {
	lua_getfield(L, -1, "args");
	int opts = get_int_element(L, 1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "handlers");
 	int hid = parse_handlers(L, env_table_index, app, id);
	lua_pop(L, 1);

	FXCanvas* canvas = new FXCanvas(parent, id >= 0 ? app : NULL, hid, opts);
	elem = new gui_window(canvas);
	printf("Adding canvas (id=%i) to object id= %i\n", id, parent_id);
	break;
      }
    default:
      luaL_error(L, "unknown type_id code: %i", type_id);
    }

    app->bind(id, elem);

    if (name) app->map(name, id);

    lua_pop(L, 1);
  }
}

void fox_window::create()
{
  FXMainWindow::create();
  show(PLACEMENT_SCREEN);
}

lua_fox_window::~lua_fox_window()
{
  if (status == not_ready || status == error)
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

  int thread_id;
  lua_State* L = app->get_lua_state(thread_id);

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
  window_index_remove (L, thread_id);
  GSL_SHELL_UNLOCK();

  lwin->status = lua_fox_window::closed;
  delete app;

  return NULL;
}

int fox_window_run(lua_State* L)
{
  lua_fox_window* lwin = object_check<lua_fox_window>(L, 1, GS_FOX_WINDOW);
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

int fox_window_handle_msg(lua_State* L)
{
  lua_fox_window* lwin = object_check<lua_fox_window>(L, 1, GS_FOX_WINDOW);
  int id = luaL_checkinteger(L, 2);

  gui_element* elem = lwin->app()->lookup(id);

  if (!elem)
    return luaL_error(L, "invalid element id: %i in handle method", id);

  if (lwin->status == lua_fox_window::running)
    {
      gslshell::ret_status st;
      int n = elem->handle(L, st);

      if (st.error_msg()) {
	return luaL_error(L, "%s in %s\n", st.error_msg(), st.context());
      }

      return n;
    }

  return luaL_error(L, "window is not running");
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
  FXDrawable* draw = app->get_object_by_id(id);
  app->set_dc(draw);
  return 0;
}

int fox_window_dc_handle(lua_State* L)
{
  lua_fox_window* lwin = object_check<lua_fox_window>(L, 1, GS_FOX_WINDOW);
  int oper_id = lua_tointeger(L, 2);
  
  FXDCWindow* dc = lwin->app()->current_dc();

  if (!dc)
    return luaL_error(L, "cannot perform drawing operation");

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
    return luaL_error(L, "invalid drawing operation id: %i", oper_id);
  }

  return 0;
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
