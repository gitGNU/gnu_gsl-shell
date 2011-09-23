
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

static const struct luaL_Reg fox_window_functions[] = {
  {"fox_window", fox_window_new},
  {NULL, NULL}
};

static const struct luaL_Reg fox_window_methods[] = {
  {"run",       fox_window_run},
  {"element",   fox_window_get_element},
  {"handle",    fox_window_handle_msg},
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
FX::FXuint parse_handlers(lua_State* L, int env_table_index, fox_app* app)
{
  FX::FXuint id;

  assert (lua_istable(L, -1));

  lua_getfield(L, -1, "id");
  id = fox_app::ID_LAST + lua_tointeger(L, -1);
  lua_pop(L, 1);

  int hn = lua_objlen(L, -1);
  for (int k = 1; k <= hn; k++) {
    lua_rawgeti(L, -1, k);

    lua_rawgeti(L, -1, 1);
    FX::FXuint sel = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_rawgeti(L, -1, 2);
    int env_index = app->assign_handler(FXSEL(sel,id));
    lua_rawseti(L, env_table_index, env_index);

    lua_pop(L, 1);
  }

  return id;
}

fox_window::fox_window(lua_State* L, fox_app* app, const char* title, int id, int opts, int w, int h)
  : FXMainWindow(app, title, NULL, NULL, opts, 0, 0, w, h)
{
  /* position in the Lua stack of the environment table for the window */
  const int env_table_index = 3;

  app->bind(id, new fake_element<fox_window>(this));

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
	elem = new fake_element<FXHorizontalFrame>(hf);
	printf("Adding horizontal frame (id=%i) to object id= %i\n", id, parent_id);
	break;
      }
    case gui::vertical_frame: 
      {
	lua_getfield(L, -1, "args");
	int opts = get_int_element(L, 1);
	lua_pop(L, 1);
	FXVerticalFrame* hf = new FXVerticalFrame(parent, opts);
	elem = new fake_element<FXVerticalFrame>(hf);
	printf("Adding vertical frame (id=%i) to object id= %i\n", id, parent_id);
	break;
      }
    case gui::button:
      {
	lua_getfield(L, -1, "args");
	const char* text = get_string_element(L, 1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "handlers");
 	FXuint id = parse_handlers(L, env_table_index, app);
	lua_pop(L, 1);

	FXButton* b = new FXButton(parent, text, NULL, app, id);
	elem = new fake_element<FXButton>(b);
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
    case gui::canvas:
      {
	FXCanvas* canvas = new FXCanvas(parent);
	elem = new fake_element<FXCanvas>(canvas);
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
    lwin->window = new fox_window(L, app, title, id, opts, width, height);

    lua_pop(L, 1);
    lua_setfenv(L, -2);

    return 1;
  }

  return luaL_error (L, "invalid contructor type_id: %i", type_id);
}

int fox_window_free(lua_State* L)
{
  lua_fox_window* lwin = object_check<lua_fox_window>(L, 1, GS_FOX_WINDOW);
  fox_window* win = lwin->window;
  FXApp* app = win->getApp();

  if (lwin->status == lua_fox_window::not_ready || lwin->status == lua_fox_window::error)
    delete app;

  return 0;
}

static void *
fox_window_thread_function (void *_inf)
{
  lua_fox_window* lwin = (lua_fox_window*) _inf;
  fox_window* win = lwin->window;
  fox_app* app = (fox_app*) win->getApp();

  int window_id;
  lua_State* L = app->get_lua_state(window_id);

  int argc = 1;
  char null[1] = {'\0'};
  char *argv[1];

  argv[0] = null;

  // Open display
  app->init(argc, argv);

  lwin->status = lua_fox_window::running;

  app->create();
  win->show(PLACEMENT_SCREEN);
  app->run();

  GSL_SHELL_LOCK();
  window_index_remove (L, window_id);
  GSL_SHELL_UNLOCK();

  lwin->status = lua_fox_window::closed;
  delete app;

  return NULL;
}

int fox_window_run(lua_State* L)
{
  lua_fox_window* lwin = object_check<lua_fox_window>(L, 1, GS_FOX_WINDOW);
  fox_window* win = lwin->window;
  fox_app* app = (fox_app*) win->getApp();

  lwin->status = lua_fox_window::starting;

  int window_id = window_index_add(L, 1);

  app->set_lua_state(L, window_id);

  pthread_attr_t attr[1];
  pthread_t win_thread[1];

  pthread_attr_init (attr);
  pthread_attr_setdetachstate (attr, PTHREAD_CREATE_DETACHED);

  void *user_data = (void *) lwin;
  if (pthread_create(win_thread, attr, fox_window_thread_function, user_data))
    {
      lwin->status = lua_fox_window::error; 
      pthread_attr_destroy (attr);
      window_index_remove (L, window_id);
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
      int n = elem->lua_call(L, st);

      if (st.error_msg()) {
	return luaL_error(L, "%s in %s\n", st.error_msg(), st.context());
      }

      return n;
    }

  return luaL_error(L, "window is not running");
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
