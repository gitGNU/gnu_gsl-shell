
#include <new>
#include <assert.h>

#include "fox_window.h"
#include "gui-types.h"
#include "gs-types.h"
// #include "lua-cpp-utils.h"

__BEGIN_DECLS

static int fox_window_run(lua_State* L);
static int fox_window_new(lua_State* L);
static int fox_window_free(lua_State* L);

static const struct luaL_Reg fox_window_functions[] = {
  {"fox_window", fox_window_new},
  {NULL, NULL}
};

static const struct luaL_Reg fox_window_methods[] = {
  {"run",       fox_window_run},
  {"__gc",      fox_window_free},
  {NULL, NULL}
};

__END_DECLS

const FX::FXMetaClass fox_app::metaClass("fox_app", fox_app::manufacture, &FXApp::metaClass, NULL, 0, 0);

long fox_app::handle(FX::FXObject* sender,FX::FXSelector sel,void* ptr)
{ 
  return FXApp::handle(sender, sel, ptr);
  //  const FXMapEntry* me=(const FXMapEntry*)metaClass.search(sel);
  //  return me ? (this->* me->func)(sender,sel,ptr) : FXMainWindow::handle(sender,sel,ptr); 
};

int get_int_field (lua_State* L, const char* key)
{
  lua_getfield(L, -1, key);
  int result = lua_tointeger(L, -1);
  lua_pop(L, 1);
  return result;
}

/*
bool get_opt_int_field (lua_State* L, const char* key, int& value)
{
  lua_getfield(L, -1, key);

  if (lua_isnumber(L, -1)) {
    value = lua_tointeger(L, -1);
    lua_pop(L, 1);
    return true;
  }

  lua_pop(L, 1);
  return false;
}
*/

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

fox_window::fox_window(lua_State* L, fox_app* app, const char* title, int id, int opts, int w, int h)
  : FXMainWindow(app, title, NULL, NULL, opts, 0, 0, w, h)
{
  app->bind(id, this);

  int n = lua_objlen(L, -1);

  for (int j = 1; j <= n; j++) {
    lua_rawgeti(L, -1, j);

    int type_id = get_int_field(L, "type_id");
    int id = get_int_field(L, "id");
    const char* name = get_string_field(L, "name");

    int parent_id = get_int_field(L, "parent");
    FXComposite* parent = (FXComposite*) app->get_object_by_id(parent_id);

    assert(parent);

    switch (type_id) {
    case gui::horizontal_frame: 
      {
	lua_getfield(L, -1, "args");
	int opts = get_int_element(L, 1);
	lua_pop(L, 1);
	FXHorizontalFrame* hf = new FXHorizontalFrame(parent, opts);
	printf("Adding horizontal frame (id=%i) to object id= %i\n", id, parent_id);
	app->bind(id, hf);
	break;
      }
    case gui::vertical_frame: 
      {
	lua_getfield(L, -1, "args");
	int opts = get_int_element(L, 1);
	lua_pop(L, 1);
	FXVerticalFrame* hf = new FXVerticalFrame(parent, opts);
	printf("Adding vertical frame (id=%i) to object id= %i\n", id, parent_id);
	app->bind(id, hf);
	break;
      }
    case gui::button:
      {
	lua_getfield(L, -1, "args");
	const char* text = get_string_element(L, 1);
	lua_pop(L, 1);
	FXButton* b = new FXButton(parent, text);
	printf("Adding button (id=%i) to object id= %i\n", id, parent_id);
	app->bind(id, b);
	break;
      }
    case gui::text_field:
      {
	lua_getfield(L, -1, "args");
	int columns = get_int_element(L, 1);
	lua_pop(L, 1);
	FXTextField* tf = new FXTextField(parent, columns);
	printf("Adding text field (id=%i) to object id= %i\n", id, parent_id);
	app->bind(id, tf);
	break;
      }
    case gui::canvas:
      {
	FXCanvas* canvas = new FXCanvas(parent);
	printf("Adding canvas (id=%i) to object id= %i\n", id, parent_id);
	app->bind(id, canvas);
	break;
      }
    default:
      luaL_error(L, "unknown type_id code: %i", type_id);
    }

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

    fox_app* app = new fox_app();
    void* win_buf = gs_new_object(sizeof(fox_window), L, GS_FOX_WINDOW);

    lua_getfield(L, -2, "body");
    fox_window* win = new(win_buf) fox_window(L, app, title, id, opts, width, height);
    lua_pop(L, 1);

    return 1;
  }

  return luaL_error (L, "invalid contructor type_id: %i", type_id);
}

int fox_window_free(lua_State* L)
{
  fox_window* win = (fox_window*) gs_check_userdata (L, 1, GS_FOX_WINDOW);
  win->~fox_window();
  return 0;
}

int fox_window_run(lua_State* L)
{
  fox_window* win = (fox_window*) gs_check_userdata (L, 1, GS_FOX_WINDOW);
  FXApp* app = win->getApp();

  int argc = 1;
  char *argv[] = {"prova"};

  // Open display
  app->init(argc, argv);
  app->create();
  win->show(PLACEMENT_SCREEN);
  app->run();
  return 0;
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
