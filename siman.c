
/* siman.c
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
#include <gsl/gsl_siman.h>

#include "gs-types.h"
#include "matrix.h"
#include "random.h"

static int siman (lua_State *L);

#define MY_PAGE_SIZE (8 * 1024)
#define MY_PAGE_SIZE_DOUBLE (MY_PAGE_SIZE / sizeof(double))

#define LOCAL_EF_INDEX          2
#define LOCAL_STEPF_INDEX       3
#define LOCAL_METRICF_INDEX     4
#define LOCAL_PRINTF_INDEX      5
#define LOCAL_PAGETABLE_INDEX   6
#define LOCAL_SIMANINFO_INDEX   7
#define LOCAL_XP_INDEX          8
#define LOCAL_YP_INDEX          9
#define LOCAL_RNG_INDEX         10

static const struct luaL_Reg siman_functions[] = {
  {"siman",            siman},
  {NULL, NULL}
};  

struct chunk_node {
  struct chunk_node *next;
  double chunk[1];
};

struct siman_info {
  lua_State *L;
  size_t n;
  size_t bsize;
  size_t size1, size2;
  double *current, *page_end;
  struct chunk_node *free_list;
};

struct tag_chunk {
  struct siman_info *inf;
  double chunk[1];
};

static void
get_new_page (lua_State *L, struct siman_info *inf)
{
  gsl_matrix *m = matrix_push (L, MY_PAGE_SIZE_DOUBLE, 1);
  size_t len = lua_objlen (L, LOCAL_PAGETABLE_INDEX);
  lua_rawseti (L, LOCAL_PAGETABLE_INDEX, len+1);
  inf->current = m->data;
  inf->page_end = m->data + MY_PAGE_SIZE_DOUBLE;
}

static struct tag_chunk *
get_new_chunk (lua_State *L, struct siman_info *inf)
{
  struct tag_chunk *tagchunk;

  if (inf->current + inf->bsize > inf->page_end)
    get_new_page (L, inf);

  tagchunk = (struct tag_chunk *) inf->current;
  inf->current += inf->bsize;
  tagchunk->inf = inf;

  return tagchunk;
}

static void
pop_arg_function (lua_State *L, int index, const char *name)
{
  lua_getfield (L, index, name);
  if (! lua_isfunction (L, -1))
    luaL_error (L, "expecting a function for field \"%s\"", name);
}

void 
my_assign (void *_src, void *_dst)
{
  struct tag_chunk *srctag = _src, *dsttag = _dst;
  struct siman_info *inf = srctag->inf;
  double *src = srctag->chunk, *dst = dsttag->chunk;
  double *srclmt = src + inf->n;
  for (/* */; src < srclmt; src++, dst++)
    *dst = *src;
}

void *
my_copy (void *_src)
{
  struct tag_chunk *srctag = _src, *dsttag;
  struct siman_info *inf = srctag->inf;
  double *src, *dst, *srclmt;

  if (inf->free_list)
    {
      struct chunk_node *next = inf->free_list->next;
      dsttag = (struct tag_chunk *) inf->free_list;
      inf->free_list = next;
      dsttag->inf = inf;
    }
  else
    {
      dsttag = get_new_chunk (inf->L, inf);
    }

  src = srctag->chunk;
  dst = dsttag->chunk;

  srclmt = src + inf->n;
  for (/* */; src < srclmt; src++, dst++)
    *dst = *src;
  return dsttag;
}

void
my_destroy (void *_src)
{
  struct tag_chunk *srctag = _src;
  struct siman_info *inf = srctag->inf;
  struct chunk_node *node = (struct chunk_node *) _src;
  node->next = inf->free_list;
  inf->free_list = node;
}

static double
siman_ef_hook (void *_xp)
{
  struct tag_chunk *xptag = _xp;
  struct siman_info *inf = xptag->inf;
  lua_State *L = inf->L;
  matrix_set_view (L, LOCAL_XP_INDEX, xptag->chunk, 
		   inf->size1, inf->size2, NULL);
  lua_pushvalue (L, LOCAL_EF_INDEX);
  lua_pushvalue (L, LOCAL_XP_INDEX);
  lua_call (L, 1, 1);
  return lua_tonumber(L, -1);
}

static void
siman_step_hook (const gsl_rng *r, void *_xp, double step_size)
{
  struct tag_chunk *xptag = _xp;
  struct siman_info *inf = xptag->inf;
  lua_State *L = inf->L;
  matrix_set_view (L, LOCAL_XP_INDEX, xptag->chunk, inf->size1, inf->size2,
		   NULL);
  lua_pushvalue (L, LOCAL_STEPF_INDEX);
  lua_pushvalue (L, LOCAL_RNG_INDEX);
  lua_pushvalue (L, LOCAL_XP_INDEX);
  lua_pushnumber (L, step_size);
  lua_call (L, 3, 0);
}

static double
siman_metric_hook (void *_xp, void *_yp)
{
  struct tag_chunk *xptag = _xp, *yptag = _yp;
  struct siman_info *inf = xptag->inf;
  lua_State *L = inf->L;
  matrix_set_view (L, LOCAL_XP_INDEX, xptag->chunk, inf->size1, inf->size2,
		   NULL);
  matrix_set_view (L, LOCAL_YP_INDEX, yptag->chunk, inf->size1, inf->size2,
		   NULL);
  lua_pushvalue (L, LOCAL_METRICF_INDEX);
  lua_pushvalue (L, LOCAL_XP_INDEX);
  lua_pushvalue (L, LOCAL_YP_INDEX);
  lua_call (L, 2, 1);
  return lua_tonumber(L, -1);
}

static void
siman_print_hook (void *_xp)
{
  struct tag_chunk *xptag = _xp;
  struct siman_info *inf = xptag->inf;
  lua_State *L = inf->L;
  matrix_set_view (L, LOCAL_XP_INDEX, xptag->chunk, inf->size1, inf->size2,
		   NULL);
  lua_pushvalue (L, LOCAL_PRINTF_INDEX);
  lua_pushvalue (L, LOCAL_XP_INDEX);
  lua_call (L, 1, 0);
}

int
siman (lua_State *L)
{
  gsl_rng *r;
  size_t i, j, s1, s2;
  gsl_matrix *x0;
  struct siman_info *inf;
  struct tag_chunk *x0tag;
  gsl_siman_params_t p = {
    .n_tries = 200,
    .iters_fixed_T = 1000,
    .step_size = 1.0,
    .k = 1.0,
    .t_initial = 0.008,
    .mu_t = 1.003, 
    .t_min = 2.0e-6
  };

  if (! lua_istable (L, 1))
    return luaL_error (L, "expect table with fields: E, step, distance");

  pop_arg_function (L, 1, "E");
  pop_arg_function (L, 1, "step");
  pop_arg_function (L, 1, "distance");
  pop_arg_function (L, 1, "print");

  lua_getfield (L, 1, "x0");
  x0 = matrix_check (L, 6);
  lua_pop (L, 1);

  s1 = x0->size1;
  s2 = x0->size2;

  /* to store the matrices for page allocations */
  lua_newtable (L);

  inf = lua_newuserdata (L, sizeof(struct siman_info));
  inf->n = s1 * s2;
  inf->bsize = s1 * s2 + 1;
  inf->size1 = s1;
  inf->size2 = s2;
  inf->free_list = NULL;
  inf->L = L;

  get_new_page (L, inf);

  x0tag = get_new_chunk (L, inf);
  for (i = 0; i < s1; i++)
    for (j = 0; j < s2; j++)
      x0tag->chunk[i*s2 + j] = gsl_matrix_get (x0, i, j);

  matrix_push_view (L, NULL);
  matrix_push_view (L, NULL);

  r = push_rng (L, gsl_rng_taus2);

  gsl_siman_solve (r, x0tag, 
		   siman_ef_hook, siman_step_hook, siman_metric_hook, 
		   siman_print_hook,
		   my_assign, my_copy, my_destroy, 0, p);

  for (i = 0; i < s1; i++)
    for (j = 0; j < s2; j++)
      gsl_matrix_set (x0, i, j, x0tag->chunk[i*s2 + j]);

  return 0;
}

void
siman_register (lua_State *L)
{
  luaL_register (L, NULL, siman_functions);
}
