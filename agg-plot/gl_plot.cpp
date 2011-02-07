
#include "util/agg_color_conv_rgb8.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

#include "lua-cpp-utils.h"
#include "window_registry.h"
#include "window.h"
#include "gl_plot.h"
#include "gl_plot_cpp.h"
#include "rect.h"
#include "colors.h"

static int gl_plot_new       (lua_State *L);
static int gl_plot_free      (lua_State *L);
static int gl_plot_new_list  (lua_State *L);
static int gl_plot_end_list  (lua_State *L);
static int gl_plot_begin     (lua_State *L);
static int gl_plot_end       (lua_State *L);
static int gl_plot_vertex    (lua_State *L);
static int gl_plot_normal    (lua_State *L);
static int gl_plot_model_rot (lua_State *L);

static void gl_plot_update_raw (lua_State *L, gl_plot *p, int plot_index);

static const struct luaL_Reg gl_plot_functions[] = {
  {"glplot",        gl_plot_new},
  {NULL, NULL}
};

static const struct luaL_Reg gl_plot_methods[] = {
  {"__gc",          gl_plot_free       },
  {"start",         gl_plot_new_list   },
  {"close",         gl_plot_end_list   },
  {"vbegin",        gl_plot_begin   },
  {"vend",          gl_plot_end   },
  {"vertex",        gl_plot_vertex   },
  {"normal",        gl_plot_normal   },
  {"rotate",        gl_plot_model_rot},
  {NULL, NULL}
};

void
gl_plot::draw(canvas &canvas, agg::trans_affine& m)
{
  agg::rendering_buffer glbuf;
  agg::rect_base<int> r = rect_of_slot_matrix<int>(m);
  unsigned w = r.x2 - r.x1, h = r.y2 - r.y1;
  m_gl_ren.draw(glbuf, w, h);
  canvas.draw_buffer(&glbuf, r.x1, r.y1, agg::color_conv_rgb24_to_bgr24());
}

int
gl_plot_new (lua_State *L)
{
  gl_plot *p = push_new_object<gl_plot>(L, GS_GLPLOT);
  return 1;
}

int
gl_plot_free (lua_State *L)
{
  return object_free<gl_plot>(L, 1, GS_GLPLOT);
}

int
gl_plot_new_list (lua_State *L)
{
  gl_plot *p = object_check<gl_plot>(L, 1, GS_GLPLOT);
  agg::rgba8 c = color_arg_lookup (L, 2);
  p->renderer().new_list(c);
  return 0;
}

int
gl_plot_end_list (lua_State *L)
{
  gl_plot *p = object_check<gl_plot>(L, 1, GS_GLPLOT);
  gl_renderer& ren = p->renderer();
  if (ren.list_is_open())
    ren.end_list();
  gl_plot_update_raw (L, p, 1);
  return 0;
}

int
gl_plot_begin (lua_State *L)
{
  gl_plot *p = object_check<gl_plot>(L, 1, GS_GLPLOT);
  const char *tp = lua_tostring (L, 2);

  if (tp == 0)
    return gs_type_error (L, 2, "string");

  gl_renderer& ren = p->renderer();
  if (! ren.list_is_open())
    return luaL_error (L, "no opened list");

  if (! ren.begin(tp))
    return luaL_error (L, "wrong element type");

  return 0;
}

int
gl_plot_end (lua_State *L)
{
  gl_plot *p = object_check<gl_plot>(L, 1, GS_GLPLOT);
  p->renderer().end();
  return 0;
}

int
gl_plot_vertex (lua_State *L)
{
  gl_plot *p = object_check<gl_plot>(L, 1, GS_GLPLOT);
  double x = gs_check_number (L, 2, true);
  double y = gs_check_number (L, 3, true);
  double z = gs_check_number (L, 4, true);

  p->renderer().bbox(x, y, z);

  glVertex3f(x, y, z);
  return 0;
}

int
gl_plot_normal (lua_State *L)
{
  gl_plot *p = object_check<gl_plot>(L, 1, GS_GLPLOT);
  double x = gs_check_number (L, 2, true);
  double y = gs_check_number (L, 3, true);
  double z = gs_check_number (L, 4, true);
  glNormal3f(x, y, z);
  return 0;
}

int
gl_plot_model_rot (lua_State *L)
{
  gl_plot *p = object_check<gl_plot>(L, 1, GS_GLPLOT);
  double ax = gs_check_number (L, 2, true);
  double ay = gs_check_number (L, 3, true);
  double az = gs_check_number (L, 4, true);
  p->renderer().set_rotation(ax, ay, az);
  gl_plot_update_raw (L, p, 1);
  return 0;
}

void
gl_plot_update_raw (lua_State *L, gl_plot *p, int plot_index)
{
  window_refs_lookup_apply (L, plot_index, window_slot_update);
}

void
gl_plot_register (lua_State *L)
{
  luaL_newmetatable (L, GS_METATABLE(GS_GLPLOT));
  lua_pushvalue (L, -1);
  lua_setfield (L, -2, "__index");
  luaL_register (L, NULL, gl_plot_methods);
  lua_pop (L, 1);

  luaL_register (L, NULL, gl_plot_functions);
}
