// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2009 Simon Goodall

#ifndef SEAR_LUASIGC_H
#define SEAR_LUASIGC_H 1

#include <string>
#include <cstdio>

extern "C" {
  #include "lua.h"
  #include "lualib.h"
  #include "lauxlib.h"
}

#include "LuaScriptEngine.h"
#include "LuaHelper.hpp"

namespace Sear {

typedef struct {
  lua_State *L;
  int top;
  int errorIndex;
  SWIGLUA_REF ref;
  int numArgs;
} CallState;

static void begin(CallState *state)
{
  state->top = lua_gettop(state->L);

  // Push Error function 
  lua_pushcfunction(state->L, LuaScriptEngine::stackDump);

  state->errorIndex = state->top + 1;

  // Setup function
  swiglua_ref_get(&state->ref);
}

static void end(CallState *state) {
  // Run command
  int error = lua_pcall(state->L, state->numArgs, LUA_MULTRET, state->errorIndex);

  if (error) {
    // TODO: GUI Error report?
    LuaScriptEngine::printLuaError(error);
    fprintf(stderr, "[Lua] An error occurred during callback\n");
  }
  
  // Restore stack position
  lua_settop(state->L, state->top);
}

static void callback0(lua_State *L, SWIGLUA_REF ref) {

  CallState state;
  state.L = L;
  state.ref = ref;
  
  begin(&state);

  state.numArgs = 0;

  end(&state);
}

template<typename T>
static void callback1(const T &t, lua_State *L, SWIGLUA_REF ref) {

  CallState state;
  state.L = L;
  state.ref = ref;
  
  begin(&state);

  // Push arguments
  state.numArgs = 0;
  pushvalue(L, t); state.numArgs++;

  end(&state);
}

};

#endif // SEAR_LUASIGC_H
