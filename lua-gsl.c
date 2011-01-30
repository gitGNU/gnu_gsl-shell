
/* lua-gsl.c
 * 
 * Copyright (C) 2009 Francesco Abbate
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <lua.h>
#include <lauxlib.h>

#include "lua-gsl.h"
#include "gs-types.h"
#include "lua-utils.h"
#include "nlinfit.h"
#include "cnlinfit.h"
#include "matrix.h"
#include "cmatrix.h"
#include "matrix_arith.h"
#include "linalg.h"
#include "integ.h"
#include "fft.h"
#include "ode_solver.h"
#include "ode.h"
#include "code.h"
#include "random.h"
#include "randist.h"
#include "pdf.h"
#include "cdf.h"
#include "sf.h"
#include "multimin.h"
#include "eigen-systems.h"
#include "mlinear.h"
#include "bspline.h"
#include "interp.h"

#ifdef AGG_PLOT_ENABLED
#include "object-index.h"
#include "object-refs.h"
#include "lua-draw.h"
#include "lua-text.h"
#include "window.h"
#include "gl_plot.h"
#include "lua-plot.h"
#endif

static const struct luaL_Reg gsl_methods_dummy[] = {{NULL, NULL}};

int
luaopen_gsl (lua_State *L)
{
  gsl_set_error_handler_off ();

#ifdef AGG_PLOT_ENABLED
  object_index_prepare (L);
  object_refs_prepare (L);
#endif

#ifdef USE_SEPARATE_NAMESPACE
  luaL_register (L, MLUA_GSLLIBNAME, gsl_methods_dummy);
#else
  lua_pushvalue (L, LUA_GLOBALSINDEX);
#endif

  luaL_register (L, NULL, gs_type_functions);

  solver_register (L);
  matrix_register (L);
  matrix_arith_register (L);
  linalg_register (L);
  integ_register (L);
  ode_register (L);
  random_register (L);
  randist_register (L);
  pdf_register (L);
  cdf_register (L);
  sf_register (L);
  multimin_register (L);
  eigen_register (L);
  mlinear_register (L);
  bspline_register (L);
  interp_register (L);
#ifdef AGG_PLOT_ENABLED
  draw_register (L);
  text_register (L);
  window_register (L);
  gl_plot_register (L);
  plot_register (L);
#endif

#ifdef LNUM_COMPLEX
  lua_pushboolean (L, 1);
  lua_setfield (L, -2, "have_complex");

  fft_register (L);
  matrix_complex_register (L);
  ode_complex_register (L);
  solver_complex_register (L);
#else
  lua_pushboolean (L, 0);
  lua_setfield (L, -2, "have_complex");
#endif
  lua_pop (L, 1);

  return 1;
}
