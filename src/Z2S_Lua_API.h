#pragma once

#include "Z2S_Lua_Config.h"

#if Z2S_ENABLE_LUA
extern "C" {
#include "../extras/lua55/lua.h"
#include "../extras/lua55/lauxlib.h"
#include "../extras/lua55/lualib.h"
}
#endif