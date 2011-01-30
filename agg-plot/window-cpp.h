#ifndef WINDOW_CPP_H
#define WINDOW_CPP_H

extern "C" {
#include "lua.h"
}

#include "window.h"
#include "canvas-window-cpp.h"
#include "resource-manager.h"
#include "lua-cpp-utils.h"
#include "vplot.h"
#include "drawable.h"
#include "rect.h"
#include "my_list.h"

#include "agg_color_rgba.h"
#include "agg_trans_affine.h"
#include "split-parser.h"

class window : public canvas_window {
public:
  typedef vplot plot_type;

  int window_id;

  typedef agg::trans_affine bmatrix;

  struct ref {
    typedef tree::node<ref, direction_e> node;

    plot_type *plot;
    int slot_id;

    bmatrix matrix;

    unsigned char *layer_buf;
    agg::rendering_buffer layer_img;

    bool valid_rect;
    opt_rect<double> dirty_rect;

    ref(plot_type *p = 0)
      : plot(p), matrix(), layer_buf(0), valid_rect(true), dirty_rect()
    {};

    ~ref() { if (layer_buf) delete layer_buf; };

    void save_image (agg::rendering_buffer& winbuf, agg::rect_base<int>& r,
		     int bpp, bool flip_y);

    static void compose(bmatrix& a, const bmatrix& b);
    static int calculate(node *t, const bmatrix& m, int id);
  };

private:
  void draw_slot_by_ref(ref& ref, bool dirty);
  void refresh_slot_by_ref(ref& ref, bool draw_all);
  void draw_rec(ref::node *n);
  void cleanup_tree_rec (lua_State *L, int window_index, ref::node* n);

  static ref *ref_lookup (ref::node *p, int slot_id);

  ref::node* m_tree;

public:
  window(agg::rgba& bgcol) : canvas_window(bgcol), m_tree(0)
  {
    this->split(".");
  };

  ~window() { if (m_tree) delete m_tree; };

  bool split(const char *spec);
  int attach(plot_type *plot, const char *spec);
  void draw_slot(int slot_id, bool update_req);
  void refresh_slot(int slot_id);
  void start(lua_State *L, gslshell::ret_status& st);

  void save_slot_image(int slot_id);
  void restore_slot_image(int slot_id);

  void cleanup_refs(lua_State *L, int window_index)
  {
    cleanup_tree_rec (L, window_index, m_tree);
  };

  void draw_slot(int slot_id);

  virtual void on_draw();
  virtual void on_resize(int sx, int sy);
};

#endif
