
#include "util/agg_color_conv_rgb8.h"

#include "gl-window.h"
#include "gl-window-cpp.h"

#include "object-index.h"
#include "colors.h"

#include "gears.h"

static int gl_window_new (lua_State *L);
static int gl_window_free (lua_State *L);

static const struct luaL_Reg gl_window_functions[] = {
  {"GLWindow",        gl_window_new},
  {NULL, NULL}
};

static const struct luaL_Reg gl_window_methods[] = {
  {"__gc",        gl_window_free       },
  {NULL, NULL}
};

int
gl_window::resize_viewport(GLContext *c, int *xsize_ptr, int *ysize_ptr)
{
  gl_window *win = (gl_window *) c->opaque;

  int xsize = *xsize_ptr;
  int ysize = *ysize_ptr;

  /* we ensure that xsize and ysize are multiples of 2 for the zbuffer. 
     TODO: find a better solution */
  //  xsize &= ~3;
  //  ysize &= ~3;

  if (xsize == 0 || ysize == 0) return -1;

  *xsize_ptr = xsize;
  *ysize_ptr = ysize;
  
  win->m_xsize = xsize;
  win->m_ysize = ysize;

  ZB_resize(c->zb, NULL, xsize, ysize);

  return 0;
}

void
gl_window::start (lua_State *L, gslshell::ret_status& st)
{
  int xs = 480, ys = 480;
  m_zbuf = ZB_open(xs, ys, TINYGL_MODE, 0, NULL, NULL, NULL);

  /* initialisation of the TinyGL interpreter */
  glInit(m_zbuf);

  m_gl_context = gl_get_context();
  m_gl_context->opaque = (void *) this;
  m_gl_context->gl_resize_viewport = gl_window::resize_viewport;

  /* set the viewport : we force a call to glX_resize_viewport */
  m_gl_context->viewport.xsize = -1;
  m_gl_context->viewport.ysize = -1;

  glViewport(0, 0, xs, ys);

  this->lock();

  if (status != canvas_window::running && status != canvas_window::starting)
    {
      typedef canvas_window::thread_info thread_info;
      std::auto_ptr<thread_info> inf(new thread_info(L, this));

      this->window_id = object_index_add (L, -1);
      inf->window_id = this->window_id;

      if (! this->start_new_thread (inf))
	{
	  object_index_remove (L, this->window_id);
	  this->unlock();
	  st.error("error during thread initialization", "window creation");
	}
    }
  else
    {
      this->unlock();
      st.error("window is already active", "window creation");
    }
}

void
gl_window::on_draw()
{
  canvas_window::on_draw();
  gear_draw();

  agg::rendering_buffer rbuf_win;
  this->attach_tinygl(rbuf_win);

  agg::rendering_buffer rbuf_tgl;
  unsigned w = rbuf_win.width(), h = rbuf_win.height();
  rbuf_tgl.attach((unsigned char *) m_zbuf->pbuf, w, h, m_zbuf->linesize);

  //rbuf_win.copy_from(rbuf_tgl);
  my_color_conv(&rbuf_win, &rbuf_tgl, agg::color_conv_rgb24_to_bgr24());

  do_window_update();
}

#if 0
void
gl_window::on_draw()
{
  canvas_window::on_draw();
  gear_draw();

  agg::rendering_buffer rbuf_win;
  this->attach_tinygl(rbuf_win);
  ZB_copyFrameBuffer(m_zbuf, rbuf_win.row_ptr(0), rbuf_win.stride());
  do_window_update();
}
#endif

void
gl_window::on_init()
{
  printf("on init\n");
  canvas_window::on_init();
  gear_init ();
}

void
gl_window::on_resize(int sx, int sy)
{
  printf("on resize\n");
  canvas_window::on_resize(sx, sy);
  gear_reshape(sx, sy);
}

int
gl_window_new (lua_State *L)
{
  gl_window *win = push_new_object<gl_window>(L, GS_GL_WINDOW, colors::black);

  gslshell::ret_status st;
  win->start(L, st);

  if (st.error_msg())
    return luaL_error (L, "%s (reported during %s)", st.error_msg(), st.context());

  return 1;
}

int
gl_window_free (lua_State *L)
{
  return object_free<gl_window>(L, 1, GS_GL_WINDOW);
}

void
gl_window_register (lua_State *L)
{
  luaL_newmetatable (L, GS_METATABLE(GS_GL_WINDOW));
  luaL_register (L, NULL, gl_window_methods);
  lua_pop (L, 1);

  luaL_register (L, NULL, gl_window_functions);
}
