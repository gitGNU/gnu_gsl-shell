#include "unpack.h"

int get_int_field (lua_State* L, const char* key)
{
  lua_getfield(L, -1, key);
  int result = lua_tointeger(L, -1);
  lua_pop(L, 1);
  return result;
}

const char* get_string_field (lua_State* L, const char* key)
{
  lua_getfield(L, -1, key);
  const char* result = lua_tostring(L, -1);
  lua_pop(L, 1);
  return result;
}

int get_int_element (lua_State* L, int index)
{
  lua_rawgeti(L, -1, index);
  int result = lua_tointeger(L, -1);
  lua_pop(L, 1);
  return result;
}

const char* get_string_element (lua_State* L, int index)
{
  lua_rawgeti(L, -1, index);
  const char* result = lua_tostring(L, -1);
  lua_pop(L, 1);
  return result;
}
