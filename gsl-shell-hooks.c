#include "gsl-shell.h"

#include <stdlib.h>
#include <stdio.h>

#include "object-index.h"

pthread_mutex_t gsl_shell_mutex[1];

struct window_unref_cell {
  int id;
  struct window_unref_cell *next;
};

#define UNREF_FIXED_SIZE 8
static int unref_fixed_list[UNREF_FIXED_SIZE];
static size_t unref_fixed_count = 0;

static struct window_unref_cell *window_unref_list = NULL;

static void l_message (const char *pname, const char *msg) {
  if (pname) fprintf(stderr, "%s: ", pname);
  fprintf(stderr, "%s\n", msg);
  fflush(stderr);
}

int
error_report (lua_State *L, int status)
{
  if (status && !lua_isnil(L, -1)) {
    const char *msg = lua_tostring(L, -1);
    if (msg == NULL) msg = "(error object is not a string)";
    l_message("gsl-shell", msg);
    lua_pop(L, 1);
  }
  return status;
}

void
gsl_shell_unref_plot (int id)
{
  if (unref_fixed_count < UNREF_FIXED_SIZE)
    {
      unref_fixed_list[unref_fixed_count] = id;
      unref_fixed_count ++;
    }
  else
    {
      struct window_unref_cell *cell = malloc(sizeof(struct window_unref_cell));

      cell->id = id;
      cell->next = window_unref_list;

      window_unref_list = cell;
    }
}

void do_windows_unref (lua_State *L)
{
  struct window_unref_cell *wu;
  size_t j;

  GSL_SHELL_LOCK();

  for (j = 0; j < unref_fixed_count; j++)
    {
      object_index_remove (L, OBJECT_WINDOW, unref_fixed_list[j]);
    }

  unref_fixed_count = 0;

  for (wu = window_unref_list; wu != NULL; /* */)
    {
      struct window_unref_cell *nxt = wu->next;
      object_index_remove (L, OBJECT_WINDOW, wu->id);
      free (wu);
      wu = nxt;
    }
  window_unref_list = NULL;

  GSL_SHELL_UNLOCK();
}
