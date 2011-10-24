
#include <new>
#include <assert.h>

#include "fox_window.h"
#include "gui-types.h"
#include "gs-types.h"
#include "gui_element.h"
#include "unpack.h"

// unpack the table on top of the stack to assign the handlers:
// SEL => (envinronment index)
int parse_handlers(lua_State* L, int env_table_index, fox_lua_handler* hnd, int id)
{
  if (!lua_istable(L, -1))
    return (-1);

  FX::FXuint fox_id = hnd->id_last() + id;

  int hn = lua_objlen(L, -1);
  for (int k = 1; k <= hn; k++) {
    lua_rawgeti(L, -1, k);

    lua_rawgeti(L, -1, 1);
    FX::FXuint sel = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_rawgeti(L, -1, 2);
    int env_index = hnd->assign_handler(FXSEL(sel,fox_id));
    lua_rawseti(L, env_table_index, env_index);

    lua_pop(L, 1);
  }

  return (int) fox_id;
}

static int get_field_as_int(lua_State* L, const char* key)
{
  lua_getfield(L, -1, key);
  int opts = get_int_element(L, 1);
  lua_pop(L, 1);
  return opts;
}

static const char* get_field_as_string(lua_State* L, const char* key)
{
  lua_getfield(L, -1, key);
  const char* text = get_string_element(L, 1);
  lua_pop(L, 1);
  return text;
}

static int get_handlers(lua_State* L, int env_table_index,
			fox_lua_handler* hnd, int id)
{
  lua_getfield(L, -1, "handlers");
  int hid = parse_handlers(L, env_table_index, hnd, id);
  lua_pop(L, 1);
  return hid;
}

static void parse_init_function(lua_State* L, fox_lua_handler* hnd, int env_table_index, int id, list<widget_initializer>*& init_ls)
{
  typedef list<widget_initializer> init_cell;

  lua_getfield(L, -1, "init");
  if (lua_isfunction(L, -1)) {
    int env_index = hnd->get_handler_slot();
    lua_rawseti(L, env_table_index, env_index);
    init_ls = new init_cell(widget_initializer(id, env_index), init_ls);
  } else {
    lua_pop(L, 1);
  }
}

void window_build(lua_State* L, fox_lua_handler* hnd, FXTopWindow* win, int win_id, list<widget_initializer>*& init_ls)
{
  /* position in the Lua stack of the environment table for the window */
  int env_table_index = lua_gettop(L) - 1;

  hnd->bind(win_id, new gui_composite(win));
  hnd->map("*", win_id);

  int n = lua_objlen(L, -1);

  for (int j = 1; j <= n; j++) {
    lua_rawgeti(L, -1, j);

    int type_id = get_int_field(L, "type_id");
    int id = get_int_field(L, "id");
    const char* name = get_string_field(L, "name");

    int parent_id = get_int_field(L, "parent");
    FXComposite* parent = hnd->lookup(parent_id)->as_composite();

    assert(parent);

    gui_element* elem;

    int opts = get_int_field(L, "options");

    switch (type_id) {
    case gui::horizontal_frame: 
      {
	FXHorizontalFrame* hf = new FXHorizontalFrame(parent, opts);
	elem = new gui_composite(hf);
	break;
      }
    case gui::vertical_frame: 
      {
	FXVerticalFrame* hf = new FXVerticalFrame(parent, opts);
	elem = new gui_composite(hf);
	break;
      }
    case gui::button:
      {
	const char* text = get_field_as_string(L, "args");
	int hid = get_handlers(L, env_table_index, hnd, id);
	FXObject* target = (hid >= 0 ? win : NULL);
	FXButton* b = new FXButton(parent, text, NULL, target, hid, opts);
	elem = new gui_window(b);
	break;
      }
    case gui::text_field:
      {
	int columns = get_field_as_int(L, "args");
	int hid = get_handlers(L, env_table_index, hnd, id);
	FXObject* target = (hid >= 0 ? win : NULL);
	FXTextField* tf = new FXTextField(parent, columns, target, hid, opts);
	elem = new text_field(tf);
	break;
      }
    case gui::label:
      {
	const char *text = get_field_as_string(L, "args");
	FXLabel* label = new FXLabel(parent, text, NULL, opts);
	elem = new gui_window(label);
	break;
      }
    case gui::canvas:
      {
	int hid = get_handlers(L, env_table_index, hnd, id);
	FXObject* target = (hid >= 0 ? win : NULL);
	FXCanvas* canvas = new FXCanvas(parent, target, hid, opts);
	elem = new gui_window(canvas);
	break;
      }
    case gui::menu_bar:
      {
	FXMenuBar* mb = new FXMenuBar(parent, opts);
	elem = new gui_composite(mb);
	break;
      }
    case gui::menu_title:
      {
	lua_getfield(L, -1, "args");
	const char* text = get_string_element(L, 1);
	int menu_pane_id = get_int_element(L, 2);
	lua_pop(L, 1);

	gui_element* menu_pane_el = hnd->lookup(menu_pane_id);

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
	hnd->add_resource(mp);
	elem = new gui_composite(mp);
	break;
      }
    case gui::menu_command:
      {
	const char *text = get_field_as_string(L, "args");
	int hid = get_handlers(L, env_table_index, hnd, id);
	FXObject* target = (hid >= 0 ? win : NULL);
	FXMenuCommand* mc = new FXMenuCommand(parent, text, NULL, target, hid);
	elem = new gui_window(mc);
	break;
      }
    case gui::radio_button:
      {
	const char* text = get_field_as_string(L, "args");
	int hid = get_handlers(L, env_table_index, hnd, id);
	FXObject* target = (hid >= 0 ? win : NULL);
	FXRadioButton* b = new FXRadioButton(parent, text, target, hid, opts);
	elem = new gui_window(b);
	break;
      }
    case gui::check_button:
      {
	const char* text = get_field_as_string(L, "args");
	int hid = get_handlers(L, env_table_index, hnd, id);
	FXObject* target = (hid >= 0 ? win : NULL);
	FXCheckButton* b = new FXCheckButton(parent, text, target, hid, opts);
	elem = new gui_window(b);
	break;
      }
    case gui::combo_box:
      {
	lua_getfield(L, -1, "args");
	int ncols = get_int_element(L, 1);
	lua_pop(L, 1);

	int hid = get_handlers(L, env_table_index, hnd, id);
	FXObject* target = (hid >= 0 ? win : NULL);
	FXComboBox* b = new FXComboBox(parent, ncols, target, hid, opts);
	elem = new combo_box(b);
	break;
      }
    default:
      luaL_error(L, "unknown type_id code: %d", type_id);
    }

    parse_init_function(L, hnd, env_table_index, id, init_ls);

    hnd->bind(id, elem);

    if (name) hnd->map(name, id);

    lua_pop(L, 1);
  }
}

long fox_window::handle(FX::FXObject* sender,FX::FXSelector sel,void* ptr)
{
  long status = m_handler->handle(sender, sel, ptr);
  if (status == 0)
    return FXMainWindow::handle(sender, sel, ptr);
  return status;
}

void fox_window::create()
{
  FXMainWindow::create();
  show(PLACEMENT_SCREEN);
}

long fox_dialog::handle(FX::FXObject* sender,FX::FXSelector sel,void* ptr)
{
  long status = m_handler->handle(sender, sel, ptr);
  if (status == 0)
    return FXDialogBox::handle(sender, sel, ptr);
  return status;
}
