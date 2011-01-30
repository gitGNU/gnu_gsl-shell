#ifndef AGGPLOT_UTILS_H
#define AGGPLOT_UTILS_H

#include "agg_trans_affine.h"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif


template <typename T>
T min(T a, T b)
{
  return (a < b) ? a : b;
};

template <typename T>
T max(T a, T b)
{
  return (a > b) ? a : b;
};

extern void trans_affine_compose (agg::trans_affine& a, const agg::trans_affine& b);

template<class RenBufDst, class RenBufSrc, class CopyRow> 
void my_color_conv(RenBufDst* dst, const RenBufSrc* src, CopyRow copy_row_functor)
{
  unsigned int width  = src->width();
  unsigned int height = src->height();

  for(unsigned int y = 0; y < height; y++)
    {
      copy_row_functor(dst->row_ptr(0, y, width), src->row_ptr(y), width);
    }
}

extern agg::trans_affine identity_matrix;

#endif
