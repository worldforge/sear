// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: SEFunctions.cpp,v 1.1.2.1 2003-01-05 14:19:45 simon Exp $

#include "SEFunctions.h"

#include <unistd.h>

namespace Sear {

void script_registerFunctions(lua_State *L) {
  lua_register(L, "cd", l_cd);
}

int l_cd(lua_State *L) {
  const char *dir = lua_tostring(L, 1);
  int err = chdir(dir);
  if (err != 0) {
    lua_error(L, "Error changing dir");
  }
  return 0;
}

} /* namespace Sear */
