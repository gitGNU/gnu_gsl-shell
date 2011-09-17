#ifndef CANVAS_WINDOW_CPP_H
#define CANVAS_WINDOW_CPP_H

#include <memory>

#include "platform_support_ext.h"
#include "agg_trans_affine.h"
#include "agg_color_rgba.h"
#include "lua-cpp-utils.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

#include "defs.h"
#include "drawable.h"
#include "canvas.h"
#include "utils.h"

class canvas_window : public platform_support_ext {
protected:
  canvas *m_canvas;
  agg::rgba m_bgcolor;

  agg::trans_affine m_matrix;

public:
  typedef lua_thread_info<canvas_window> thread_info;

  enum win_status_e { not_ready, starting, running, error, closed };

  enum win_status_e status;

  canvas_window(agg::rgba& bgcol) :
    platform_support_ext(agg::pix_format_bgr24, true), 
    m_canvas(NULL), m_bgcolor(bgcol), m_matrix(), status(not_ready)
  { };

  virtual ~canvas_window() 
  {
    if (m_canvas)
      delete m_canvas;
  };

  virtual void on_init();
  virtual void on_resize(int sx, int sy);

  bool start_new_thread (std::auto_ptr<thread_info>& inf);

  void scale (agg::trans_affine& m) { trans_affine_compose (m, m_matrix); };
};

#endif
