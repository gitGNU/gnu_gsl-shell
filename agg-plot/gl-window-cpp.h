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

class gl_window :  public canvas_window {
public:
  int window_id;

  gl_window(agg::rgba& bgcol) : canvas_window(bgcol), m_zbuf(0), m_gl_context(0) { };

  virtual void on_init();
  virtual void on_draw();
  virtual void on_resize(int sx, int sy);

  void start (lua_State *L, gslshell::ret_status& st);

private:
  static int resize_viewport(GLContext *c, int *xsize_ptr, int *ysize_ptr);
  unsigned char *image_buffer() { return rbuf_window().buf(); };

  const static int TINYGL_MODE = ZB_MODE_RGB24;

  ZBuffer *m_zbuf;
  GLContext *m_gl_context;
  int m_xsize, m_ysize;
};

#endif
