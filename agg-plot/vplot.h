#ifndef AGGPLOT_VPLOT_H
#define AGGPLOT_VPLOT_H

#include "agg_trans_affine.h"

#include "canvas.h"
#include "rect.h"

struct vplot {
  virtual void draw(canvas &canvas, agg::trans_affine& m) = 0;
  virtual bool need_redraw() const = 0;
  virtual void draw_queue(canvas &canvas, agg::trans_affine& m, opt_rect<double>& bbox) = 0;
};

#endif
