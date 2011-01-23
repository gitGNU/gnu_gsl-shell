#ifndef GL_WINDOW_CPP_H
#define GL_WINDOW_CPP_H

extern "C" {
#include "lua.h"
}

#include "gl-window.h"
#include "zbuffer.h"
#include "zgl.h"

#include "canvas-window-cpp.h"
#include "agg_color_rgba.h"
#include "lua-cpp-utils.h"
#include "my_list.h"

class gl_window :  public canvas_window {
public:
  int window_id;

  gl_window(agg::rgba& bgcol) : 
    canvas_window(bgcol), m_bbox_set(false),
    m_listid_head(0), m_listid_current(-1), m_build_obj(-1),
    m_zbuf(0), m_gl_context(0)
  { 
    m_view_rot[0] = -70;
    m_view_rot[1] = 0;
    m_view_rot[2] = 20;
  };

  virtual void on_init();
  virtual void on_draw();
  virtual void on_resize(int sx, int sy);

  
  void new_list(agg::rgba8& col);
  void end_list();
  bool list_is_open () const { return m_listid_current >= 0; };
  bool begin(const char *tp);
  void end();

  void start (lua_State *L, gslshell::ret_status& st);

  void bbox(double x, double y, double z);

private:

  void gl_init();
  void gl_resize(GLfloat sx, GLfloat sy);
  void gl_draw();

  static int resize_viewport(GLContext *c, int *xsize_ptr, int *ysize_ptr);

  enum { 
    tinygl_mode = ZB_MODE_RGB24,
    tinygl_xsize = 480,
    tinygl_ysize = 480
  };

  GLfloat m_xsize, m_ysize;
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
