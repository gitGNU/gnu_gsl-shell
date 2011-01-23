
#include "util/agg_color_conv_rgb8.h"

#include "gl-window.h"
#include "gl-window-cpp.h"

#include "lua-cpp-utils.h"
#include "object-index.h"
#include "colors.h"

#include <GL/gl.h> 

static int gl_window_new      (lua_State *L);
static int gl_window_free     (lua_State *L);
static int gl_window_new_list (lua_State *L);
static int gl_window_end_list (lua_State *L);
static int gl_window_begin    (lua_State *L);
static int gl_window_end      (lua_State *L);
static int gl_window_vertex   (lua_State *L);
static int gl_window_normal   (lua_State *L);
//static int gl_window_draw     (lua_State *L);

static const struct luaL_Reg gl_window_functions[] = {
  {"glwindow",        gl_window_new},
  {NULL, NULL}
};

static const struct luaL_Reg gl_window_methods[] = {
  {"__gc",          gl_window_free       },
  {"start",         gl_window_new_list   },
  {"close",         gl_window_end_list   },
  {"Begin",         gl_window_begin   },
  {"End",           gl_window_end   },
  {"vertex",        gl_window_vertex   },
  {"normal",        gl_window_normal   },
  //  {"draw",          gl_window_draw   },
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
  // ysize &= ~3;

  if (xsize == 0 || ysize == 0) return -1;

  *xsize_ptr = xsize;
  *ysize_ptr = ysize;

  win->m_xsize = xsize;
  win->m_ysize = ysize;

  printf("Resizing ZB buffer...\n");
  ZB_resize(c->zb, NULL, xsize, ysize);

  return 0;
}

void
gl_window::start (lua_State *L, gslshell::ret_status& st)
{
  int xs = gl_window::tinygl_xsize, ys = gl_window::tinygl_ysize;
  m_zbuf = ZB_open(xs, ys, gl_window::tinygl_mode, 0, NULL, NULL, NULL);

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
gl_window::bbox(double x, double y, double z)
{
  if (! m_bbox_set)
    {
      m_bbox_set = true;
      m_pmin[0] = m_pmax[0] = x;
      m_pmin[1] = m_pmax[1] = y;
      m_pmin[2] = m_pmax[2] = z;
    }
  else
    {
      if (x < m_pmin[0]) m_pmin[0] = x;
      if (y < m_pmin[1]) m_pmin[1] = y;
      if (z < m_pmin[2]) m_pmin[2] = z;

      if (x > m_pmax[0]) m_pmax[0] = x;
      if (y > m_pmax[1]) m_pmax[1] = y;
      if (z > m_pmax[2]) m_pmax[2] = z;
    }
}

void
gl_window::on_draw()
{
  canvas_window::on_draw();
  this->gl_resize(m_xsize, m_ysize);
  this->gl_draw();

  agg::rendering_buffer& rbuf_win = this->rbuf_window();

  agg::rendering_buffer rbuf_tgl;
  unsigned bw = m_zbuf->xsize, bh = m_zbuf->ysize;
  rbuf_tgl.attach((unsigned char *) m_zbuf->pbuf, bw, bh, -m_zbuf->linesize);

  /*
  agg::pixfmt_rgb24 pixf_tgl(rbuf_tgl);

  unsigned w = this->width(), h = this->height();
  agg::trans_affine cmap = agg::trans_affine_scaling((double)w, (double)h);
  m_canvas->draw_image(pixf_tgl, cmap);
  */

  my_color_conv(&rbuf_win, &rbuf_tgl, agg::color_conv_rgb24_to_bgr24());

  //  do_window_update();
}

void
gl_window::on_init()
{
  canvas_window::on_init();
}

void
gl_window::on_resize(int sx, int sy)
{
  canvas_window::on_resize(sx, sy);
  m_xsize = sx;
  m_ysize = sy;
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
gl_window::new_list (agg::rgba8& col)
{
  GLfloat glcol[4] = {col.r/255.0, col.g/255.0, col.b/255.0, col.a/255.0};

  if (this->list_is_open()) this->end_list();

  this->gl_init();

  int id = glGenLists (1);
  glNewList (id, GL_COMPILE);
  glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, glcol);

  m_listid_current = id;
}

void
gl_window::end_list ()
{
  if (m_build_obj >= 0) glEnd();
  m_listid_head = new pod_list<int>(m_listid_current, m_listid_head);
  glEndList();
  glEnable( GL_NORMALIZE );
  // gl_resize(m_xsize, m_ysize);
}

bool
gl_window::begin (const char *tp)
{
  int gltp;

  if (strcmp (tp, "QUAD") == 0)
    {
      gltp = GL_QUADS;
    }
  else if (strcmp (tp, "QUAD_STRIP") == 0)
    {
      gltp = GL_QUAD_STRIP;
    }
  else
    {
      return false;
    }

  glBegin(gltp);
  m_build_obj = gltp;

  return true;
}

void
gl_window::end ()
{
  glEnd();
  m_build_obj = -1;
}

void
gl_window::gl_resize(GLfloat sx, GLfloat sy)
{
  printf("on_resize\n");
  GLfloat h = sy / sx, ff = 0.6;

  glViewport(0, 0, sx, sy);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum( -ff, ff, -ff * h, ff * h, 15.0, 45.0 );
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -40.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void
gl_window::gl_init()
{
  static GLfloat pos[4] = {15.0, 15.0, 15.0, 0.5};

  printf("Lighting initialization...\n");

  glLightfv( GL_LIGHT0, GL_POSITION, pos );
  //   glEnable( GL_CULL_FACE );
  glEnable( GL_LIGHTING );
  glEnable( GL_LIGHT0 );
  glEnable( GL_DEPTH_TEST );
}

void
gl_window::gl_draw()
{
  printf("drawing...\n");

  //   glEnable( GL_NORMALIZE );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glPushMatrix();

  //   glPushMatrix();
  glRotatef( m_view_rot[0], 1.0, 0.0, 0.0 );
  glRotatef( m_view_rot[1], 0.0, 1.0, 0.0 );
  glRotatef( m_view_rot[2], 0.0, 0.0, 1.0 );

  if (m_listid_head != 0)
    {
      printf("Adding normalization matrix...\n");
      GLfloat scx = 2 / (m_pmax[0] - m_pmin[0]);
      GLfloat scy = 2 / (m_pmax[1] - m_pmin[1]);
      GLfloat scz = 2 / (m_pmax[2] - m_pmin[2]);
      GLfloat mx = (m_pmax[0] + m_pmin[0]) / 2;
      GLfloat my = (m_pmax[1] + m_pmin[1]) / 2;
      GLfloat mz = (m_pmax[2] + m_pmin[2]) / 2;
      printf("min pt: %g %g %g\n", m_pmin[0], m_pmin[1], m_pmin[2]);
      printf("max pt: %g %g %g\n", m_pmax[0], m_pmax[1], m_pmax[2]);
      printf("centroid: %g %g %g\n", mx, my, mz);
      printf("scale: %g %g %g\n", scx, scy, scz);
      glTranslatef(-mx, -my, -mz);
      glScalef(scx, scy, scz / 3.0);
    }

   glPushMatrix();
   //   glTranslatef( -3.0, -2.0, 0.0 );
   //   glRotatef( angle, 0.0, 0.0, 1.0 );
   for (pod_list<int> *n = m_listid_head; n != 0; n = n->next())
     {
       glCallList(n->content());
     }
   glPopMatrix();

   //   if (m_listid_head != 0)
   //  glPopMatrix();

   glPopMatrix();
}

int
gl_window_new_list (lua_State *L)
{
  gl_window *win = object_check<gl_window>(L, 1, GS_GL_WINDOW);
  agg::rgba8 c = color_arg_lookup (L, 2);
  win->new_list(c);
  return 0;
}

int
gl_window_end_list (lua_State *L)
{
  gl_window *win = object_check<gl_window>(L, 1, GS_GL_WINDOW);
  if (win->list_is_open())
    win->end_list();
  return 0;
}

int
gl_window_begin (lua_State *L)
{
  gl_window *win = object_check<gl_window>(L, 1, GS_GL_WINDOW);
  const char *tp = lua_tostring (L, 2);

  if (tp == 0)
    return gs_type_error (L, 2, "string");

  if (! win->list_is_open())
    return luaL_error (L, "no opened list");

  if (! win->begin(tp))
    return luaL_error (L, "wrong element type");

  return 0;
}

int
gl_window_end (lua_State *L)
{
  gl_window *win = object_check<gl_window>(L, 1, GS_GL_WINDOW);
  win->end();
  return 0;
}

int
gl_window_vertex (lua_State *L)
{
  gl_window *win = object_check<gl_window>(L, 1, GS_GL_WINDOW);
  double x = gs_check_number (L, 2, true);
  double y = gs_check_number (L, 3, true);
  double z = gs_check_number (L, 4, true);

  win->bbox(x, y, z);

  glVertex3f(x, y, z);
  return 0;
}

int
gl_window_normal (lua_State *L)
{
  gl_window *win = object_check<gl_window>(L, 1, GS_GL_WINDOW);
  double x = gs_check_number (L, 2, true);
  double y = gs_check_number (L, 3, true);
  double z = gs_check_number (L, 4, true);
  glNormal3f(x, y, z);
  return 0;
}

/*
int
gl_window_draw (lua_State *L)
{
  gl_window *win = object_check<gl_window>(L, 1, GS_GL_WINDOW);
  win->gl_draw();
  return 0;
}
*/

void
gl_window_register (lua_State *L)
{
  luaL_newmetatable (L, GS_METATABLE(GS_GL_WINDOW));
  lua_pushvalue (L, -1);
  lua_setfield (L, -2, "__index");
  luaL_register (L, NULL, gl_window_methods);
  lua_pop (L, 1);

  luaL_register (L, NULL, gl_window_functions);
}
