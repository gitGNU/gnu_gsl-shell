#ifndef GL_RENDERER_H
#define GL_RENDERER_H

#include "zbuffer.h"
#include "zgl.h"

#include "agg_color_rgba.h"
#include "agg_rendering_buffer.h"
#include "my_list.h"

class gl_renderer {
public:
  gl_renderer() :
    m_bbox_set(false),
    m_listid_head(0), m_listid_current(-1), m_build_obj(-1),
    m_zbuf(0), m_gl_context(0)
  {
  //  set_rotation(- M_PI_2 * 7/9, 0.0, M_PI_2 * 2/9);
  };

  void init();
  
  void new_list(agg::rgba8& col);
  void end_list();
  bool list_is_open () const { return m_listid_current >= 0; };

  bool begin(const char *tp);
  void end();

  void bbox(double x, double y, double z);
  void set_rotation(double ax, double ay, double az)
  {
    m_view_rot[0] = ax * 180.0 / M_PI;
    m_view_rot[1] = ay * 180.0 / M_PI;
    m_view_rot[2] = az * 180.0 / M_PI;
  };

  void draw(agg::rendering_buffer& glbuf, unsigned xsize, unsigned ysize);

private:

  void gl_init();
  void gl_resize(GLfloat sx, GLfloat sy);
  void gl_draw();

  static int resize_viewport(GLContext *c, int *xsize_ptr, int *ysize_ptr);

  enum { 
    tinygl_mode = ZB_MODE_RGB24,
  };

  GLfloat m_view_rot[3];

  bool m_bbox_set;
  double m_pmin[3];
  double m_pmax[3];

  pod_list<int> *m_listid_head;
  int m_listid_current;
  int m_build_obj;

  ZBuffer *m_zbuf;
  GLContext *m_gl_context;
};

#endif
