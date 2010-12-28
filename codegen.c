
#include <lua.h>
#include <lauxlib.h>

static int fps_bytecode (lua_State *L);

static const struct luaL_Reg codegen_functions[] = {
  {"fps_bytecode",         fps_bytecode},
  {NULL, NULL}
};  


int
fps_bytecode (lua_State *L)
{
  double x = luaL_checknumber (L, 1);
  float a = (float) x;
  unsigned char *p = (unsigned char *) &a;

  lua_pushinteger (L, (int) *(p+3));
  lua_pushinteger (L, (int) *(p+2));
  lua_pushinteger (L, (int) *(p+1));
  lua_pushinteger (L, (int) *(p));
  return 4;
}

void
codegen_register (lua_State *L)
{
  luaL_register (L, NULL, codegen_functions);
}
