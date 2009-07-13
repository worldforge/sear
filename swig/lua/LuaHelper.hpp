// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2009 Simon Goodall

#ifndef SEAR_LUAHELPER_H
#define SEAR_LUAHELPER_H 1

#include <string>
#include <cstdio>

extern "C" {
  #include "lua.h"
  #include "lualib.h"
  #include "lauxlib.h"
}

namespace Sear {

/** Template method to push a value onto the stack.
 *
 */
template<typename T>
void pushvalue(lua_State *L, const T &t) {
  lua_pushvalue(L, t);
}

template<>
void pushvalue(lua_State *L, const int &t) {
  lua_pushinteger(L, t);
}

template<>
void pushvalue(lua_State *L, const bool &t) {
  lua_pushboolean(L, t);
}

template<>
void pushvalue(lua_State *L, const std::string &t) {
  lua_pushstring(L, t.c_str());
}

};

#endif // SEAR_LUAHELPER_H
