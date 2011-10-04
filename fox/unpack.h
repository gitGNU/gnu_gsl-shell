#ifndef MYFOX_UNPACK_H
#define MYFOX_UNPACK_H

#include "defs.h"

__BEGIN_DECLS
#include "lua.h"
__END_DECLS

extern int         get_int_field      (lua_State* L, const char* key);
extern const char* get_string_field   (lua_State* L, const char* key);
extern int         get_int_element    (lua_State* L, int index);
extern const char* get_string_element (lua_State* L, int index);

#endif
