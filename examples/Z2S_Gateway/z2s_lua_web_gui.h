#pragma once

#include <Z2S_Lua_Config.h>

#if Z2S_ENABLE_LUA

#include <cstdint>

void Z2S_buildLuaWebGUI();

void Z2S_registerLuaHttpEndpoints();

void Z2S_loopLuaWebGUI(bool force = false);

#endif  // Z2S_ENABLE_LUA