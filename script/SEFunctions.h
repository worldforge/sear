// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: SEFunctions.h,v 1.1.2.1 2003-01-05 14:19:45 simon Exp $

#ifndef SEAR_SCRIPT_SEFUNCTIONS_H
#define SEAR_SCRIPT_SEFUNCTIONS_H 1

extern "C" {
  #include <lua.h>
}

namespace Sear {

class System;
	
extern "C" {
  void script_registerFunctions(lua_State *L);
  int l_cd(lua_State *L);
}

} /* namespace Sear */

#endif /* SEAR_SCRIPT_SEFUNCTIONS_H */
