
#include <GL/gl.h> 

#include "gl_renderer.h"

int
gl_renderer::resize_viewport(GLContext *c, int *xsize_ptr, int *ysize_ptr)
{
//  gl_renderer *win = (gl_renderer *) c->opaque;

  int xsize = *xsize_ptr;
  int ysize = *ysize_ptr;

#if 0
  /* we ensure that xsize and ysize are multiples of 2 for the zbuffer. 
     TODO: find a better solution */
  xsize &= ~3;
  ysize &= ~3;

  if (xsize == 0 || ysize == 0) return -1;

  *xsize_ptr = xsize;
  *ysize_ptr = ysize;
#endif

  ZB_resize(c->zb, NULL, xsize, ysize);

  return 0;
}

void
gl_renderer::init ()
{
  int xs = 240, ys = 240;
  m_zbuf = ZB_open(xs, ys, gl_renderer::tinygl_mode, 0, NULL, NULL, NULL);

  /* initialisation of the TinyGL interpreter */
  glInit(m_zbuf);

  m_gl_context = gl_get_context();
  m_gl_context->opaque = (void *) this;
  m_gl_context->gl_resize_viewport = gl_renderer::resize_viewport;

  /* set the viewport : we force a call to glX_resize_viewport */
  m_gl_context->viewport.xsize = -1;
  m_gl_context->viewport.ysize = -1;

  glViewport(0, 0, xs, ys);
}

void
gl_renderer::bbox(double x, double y, double z)
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

void gl_renderer::draw(agg::rendering_buffer& glbuf, unsigned xsize, unsigned ysize)
{
  if (xsize == 0 || ysize == 0) return;

  this->gl_resize((GLfloat) xsize, (GLfloat) ysize);
  this->gl_draw();

  unsigned bw = m_zbuf->xsize, bh = m_zbuf->ysize;
  glbuf.attach((unsigned char *) m_zbuf->pbuf, bw, bh, -m_zbuf->linesize);
}

void
gl_renderer::new_list (agg::rgba8& col)
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
gl_renderer::end_list ()
{
  if (m_build_obj >= 0) glEnd();
  m_listid_head = new pod_list<int>(m_listid_current, m_listid_head);
  glEndList();
  glEnable( GL_NORMALIZE );
}

bool
gl_renderer::begin (const char *tp)
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
gl_renderer::end ()
{
  glEnd();
  m_build_obj = -1;
}

void
gl_renderer::gl_resize(GLfloat sx, GLfloat sy)
{
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
gl_renderer::gl_init()
{
  static GLfloat pos[4] = {15.0, 15.0, 15.0, 0.5};

  glLightfv( GL_LIGHT0, GL_POSITION, pos );
  //   glEnable( GL_CULL_FACE );
  glEnable( GL_LIGHTING );
  glEnable( GL_LIGHT0 );
  glEnable( GL_DEPTH_TEST );
}

void
gl_renderer::gl_draw()
{
  //   glEnable( GL_NORMALIZE );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glPushMatrix();

  //   glPushMatrix();
  glRotatef( m_view_rot[0], 1.0, 0.0, 0.0 );
  glRotatef( m_view_rot[1], 0.0, 1.0, 0.0 );
  glRotatef( m_view_rot[2], 0.0, 0.0, 1.0 );

  if (m_listid_head != 0)
    {
      GLfloat scx = 2 / (m_pmax[0] - m_pmin[0]);
      GLfloat scy = 2 / (m_pmax[1] - m_pmin[1]);
      GLfloat scz = 2 / (m_pmax[2] - m_pmin[2]);
      GLfloat mx = (m_pmax[0] + m_pmin[0]) / 2;
      GLfloat my = (m_pmax[1] + m_pmin[1]) / 2;
      GLfloat mz = (m_pmax[2] + m_pmin[2]) / 2;
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
