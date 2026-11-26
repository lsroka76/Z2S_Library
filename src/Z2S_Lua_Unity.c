#include "Z2S_Lua_Config.h"

#if Z2S_ENABLE_LUA

#include "../extras/lua55/lprefix.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LUA_CORE
#define LUA_LIB
#include "../extras/lua55/luaconf.h"

/* Internal VM symbols become static for cross-file inlining & Flash savings */
#undef LUAI_FUNC
#undef LUAI_DDEC
#undef LUAI_DDEF
#define LUAI_FUNC static
#define LUAI_DDEC(def) /* empty */
#define LUAI_DDEF static

/* Core VM (official onelua.c topological order — do not reorder) */
#include "../extras/lua55/lzio.c"
#include "../extras/lua55/lctype.c"
#include "../extras/lua55/lopcodes.c"
#include "../extras/lua55/lmem.c"
#include "../extras/lua55/lundump.c"
#include "../extras/lua55/ldump.c"
#include "../extras/lua55/lstate.c"
#include "../extras/lua55/lgc.c"
#include "../extras/lua55/llex.c"
#include "../extras/lua55/lcode.c"
#include "../extras/lua55/lparser.c"
#include "../extras/lua55/ldebug.c"
#include "../extras/lua55/lfunc.c"
#include "../extras/lua55/lobject.c"
#include "../extras/lua55/ltm.c"
#include "../extras/lua55/lstring.c"
#include "../extras/lua55/ltable.c"
#include "../extras/lua55/ldo.c"
#include "../extras/lua55/lvm.c"
#include "../extras/lua55/lapi.c"

/* Auxiliary library */
#include "../extras/lua55/lauxlib.c"

/* Safe standard libraries only (no io, os, debug, package, linit) */
#include "../extras/lua55/lbaselib.c"
#include "../extras/lua55/lmathlib.c"
#include "../extras/lua55/lstrlib.c"
#include "../extras/lua55/ltablib.c"

#else
/* Prevent empty translation unit warning while emitting 0 bytes */
typedef int z2s_lua_disabled_stub_t;
#endif