
#include <lua.h>
#include <lauxlib.h>

#define __USE_MISC
#include <sys/mman.h>

#define CODE_SEGMENT_SIZE 2048
#define CONST_SEGMENT_SIZE 2048

struct opcode {
  int nargs;
  unsigned char *code_segment;
  unsigned char *const_segment;
  
  unsigned char *segment;
};

static int opcode_new (lua_State *L);
static int opcode_emit (lua_State *L);
static int opcode_emit_address (lua_State *L);
static int opcode_register_constant (lua_State *L);
static int opcode_call (lua_State *L);

typedef double (*fp_func_t1)(double);
typedef double (*fp_func_t2)(double, double);
typedef double (*fp_func_t3)(double, double, double);

static const struct luaL_Reg opcode_functions[] = {
  {"new",               opcode_new},
  {"emit",              opcode_emit},
  {"emit_address",      opcode_emit_address},
  {"register_constant", opcode_register_constant},
  {"call",              opcode_call},
  {NULL, NULL}
};  

double
c_testf (double x, double y)
{
  return (x+y)*(x-y);
}

int
opcode_new (lua_State *L)
{
  int nargs = luaL_checkinteger (L, 1);
  unsigned int sz = CODE_SEGMENT_SIZE + CONST_SEGMENT_SIZE;

  struct opcode *opc = lua_newuserdata (L, sizeof(struct opcode));
  opc->segment = mmap(0, sz, 
		      PROT_EXEC | PROT_READ | PROT_WRITE,
		      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  opc->nargs = nargs;
  opc->code_segment  = opc->segment;
  opc->const_segment = opc->segment + CODE_SEGMENT_SIZE;

  luaL_getmetatable (L, "GSL.opcode");
  lua_setmetatable (L, -2);
  return 1;
}

int
opcode_emit (lua_State *L)
{
  struct opcode *opc = luaL_checkudata (L, 1, "GSL.opcode");
  unsigned char *seg = opc->code_segment;
  int j, n = lua_gettop (L);

  for (j = 2; j <= n; j++, seg++)
    {
      int code = luaL_checkinteger (L, j);

      if (code < 0 || code > 255)
	return luaL_error (L, "invalid opcode");

      *seg = (unsigned char) code;
      printf ("%02x ", code);
    }
  printf ("\n");

  opc->code_segment = seg;

  return 0;
}

int
opcode_emit_address (lua_State *L)
{
  struct opcode *opc = luaL_checkudata (L, 1, "GSL.opcode");
  unsigned char *seg = opc->code_segment;
  unsigned int addr = luaL_checkinteger (L, 2);
  int j;

  printf ("ADDR: ");
  for (j = 0; j < 4; j++, seg++)
    {
      int code = addr && 0xFF;
      *seg = (unsigned char) code;
      addr = addr >> 8;

      printf ("%02x ", code);
    }
  printf ("\n");

  opc->code_segment = seg;

  return 0;
}

int
opcode_register_constant (lua_State *L)
{
  struct opcode *opc = luaL_checkudata (L, 1, "GSL.opcode");
  double x = luaL_checknumber (L, 2);
  unsigned char *_ct = opc->const_segment;
  float *ct = (float *) _ct;
  *ct = (float) x;
  opc->const_segment += 4;
  lua_pushinteger (L, (int) ct);
  return 1;
}

int
opcode_call (lua_State *L)
{
  struct opcode *opc = luaL_checkudata (L, 1, "GSL.opcode");
  int n = lua_gettop (L) - 1;
  double r;

  if (opc->nargs != n)
    return luaL_error (L, "expecting %d arguments", opc->nargs);

  switch (n)
    {
    case 1:
      {
	double x = luaL_checknumber (L, 2);
	fp_func_t1 f = (fp_func_t1) (opc->segment);
	r = f(x);
      }
      break;
    case 2:
      {
	double x = luaL_checknumber (L, 2), y = luaL_checknumber (L, 3);
	fp_func_t2 f = (fp_func_t2) (opc->segment);
	r = f(x, y);
      }
      break;
    default:
      return luaL_error (L, "not supported");
    }

  lua_pushnumber (L, r);
  return 1;
}

int
opcode_register (lua_State *L)
{
  luaL_newmetatable (L, "GSL.opcode");
  lua_pop (L, 1);

  luaL_register (L, "opcode", opcode_functions);
  return 1;
}
