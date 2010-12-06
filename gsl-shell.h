#ifndef GSL_SHELL_INCLUDED
#define GSL_SHELL_INCLUDED

#include "defs.h"
#include <pthread.h>

__BEGIN_DECLS

#include "lua.h"

extern int  error_report (lua_State *L, int status);
extern void gsl_shell_unref_plot (int id);
extern void do_windows_unref (lua_State *L);

extern pthread_mutex_t gsl_shell_mutex[1];

__END_DECLS

#define GSL_SHELL_LOCK() pthread_mutex_lock (gsl_shell_mutex)
#define GSL_SHELL_UNLOCK() pthread_mutex_unlock (gsl_shell_mutex)

#endif
