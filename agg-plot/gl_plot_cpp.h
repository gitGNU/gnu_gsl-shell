#ifndef GL_PLOT_CPP_H
#define GL_PLOT_CPP_H

#include "vplot.h"
#include "gl_renderer.h"

class gl_plot : public vplot {
public:
  gl_plot() : vplot(), m_gl_ren() { m_gl_ren.init(); };

  virtual void draw(canvas &canvas, agg::trans_affine& m);
  virtual bool need_redraw() const { return false; };
  virtual void draw_queue(canvas &canvas, agg::trans_affine& m, opt_rect<double>& bbox) { };

  gl_renderer& renderer() { return m_gl_ren; };
private:
  gl_renderer m_gl_ren;
};

#endif
