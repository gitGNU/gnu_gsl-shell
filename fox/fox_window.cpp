
#include <new>
#include <assert.h>

#include "fox_window.h"
#include "gui-types.h"
#include "gs-types.h"
#include "gui_element.h"
#include "unpack.h"

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

  app->bind(win_id, new gui_composite(this));
  app->map("*", win_id);

  int n = lua_objlen(L, -1);

  for (int j = 1; j <= n; j++) {
    lua_rawgeti(L, -1, j);

    int type_id = get_int_field(L, "type_id");
    int id = get_int_field(L, "id");
    const char* name = get_string_field(L, "name");

    int parent_id = get_int_field(L, "parent");
    FXComposite* parent = app->lookup(parent_id)->as_composite();

    assert(parent);

    gui_element* elem;

    switch (type_id) {
    case gui::horizontal_frame: 
      {
	lua_getfield(L, -1, "args");
	int opts = get_int_element(L, 1);
	lua_pop(L, 1);
	FXHorizontalFrame* hf = new FXHorizontalFrame(parent, opts);
	elem = new gui_composite(hf);
	printf("Adding horizontal frame (id=%i) to object id= %i\n", id, parent_id);
	break;
      }
    case gui::vertical_frame: 
      {
	lua_getfield(L, -1, "args");
	int opts = get_int_element(L, 1);
	lua_pop(L, 1);
	FXVerticalFrame* hf = new FXVerticalFrame(parent, opts);
	elem = new gui_composite(hf);
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

	FXObject* target = (hid >= 0 ? app : NULL);
	FXButton* b = new FXButton(parent, text, NULL, target, hid);

	elem = new gui_window(b);
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
	elem = new gui_window(label);
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

	FXObject* target = (hid >= 0 ? app : NULL);
	FXCanvas* canvas = new FXCanvas(parent, target, hid, opts);
	elem = new gui_window(canvas);
	printf("Adding canvas (id=%i) to object id= %i\n", id, parent_id);
	break;
      }
    case gui::menu_bar:
      {
	lua_getfield(L, -1, "args");
	int opts = get_int_element(L, 1);
	lua_pop(L, 1);
	
	FXMenuBar* mb = new FXMenuBar(parent, opts);
	elem = new gui_composite(mb);
	break;
      }
    case gui::menu_title:
      {
	lua_getfield(L, -1, "args");
	const char* text = get_string_element(L, 1);
	int menu_pane_id = get_int_element(L, 2);
	int opts = get_int_element(L, 3);
	lua_pop(L, 1);

	gui_element* menu_pane_el = app->lookup(menu_pane_id);

	if (!menu_pane_el)
	  luaL_error(L, "error in menu construction");

	FXMenuPane* menu_pane = (FXMenuPane *) (menu_pane_el->as_composite());

	FXMenuTitle* mt = new FXMenuTitle(parent, text, NULL, menu_pane, opts);
	elem = new gui_window(mt);
	break;
      }
    case gui::menu_pane:
      {
	FXMenuPane* mp = new FXMenuPane(parent);
	app->add_resource(mp);
	elem = new gui_composite(mp);
	break;
      }
    case gui::menu_command:
      {
	lua_getfield(L, -1, "args");
	const char* text = get_string_element(L, 1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "handlers");
 	int hid = parse_handlers(L, env_table_index, app, id);
	lua_pop(L, 1);

	FXObject* target = (hid >= 0 ? app : NULL);
	FXMenuCommand* mc = new FXMenuCommand(parent, text, NULL, target, hid);
	elem = new gui_window(mc);
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
