// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _DEBUG_H_
#define _DEBUG_H_ 1

#define ENABLE_DEBUG
#ifdef ENABLE_DEBUG

#include <iostream.h>
#include <string>
#include <time.h>
//#include <stdlib.h>
//#include "Types.h"

template <typename T>
inline void DEBUG_PRINT(const T &t) { std::cout << t << std::endl; std::cout.flush(); }
/*
inline void DEBUG(const std::string & s) { cout << s << endl; cout.flush(); }
inline void DEBUG(time_t & t) { cout << "Time: " << t <<endl; cout.flush(); }
inline void DEBUG(double &d) { cout << d << endl; cout.flush(); }
inline void DEBUG(Uint32 &ui) { cout << ui << endl; cout.flush(); }
inline void DEBUG(Coord &c) { cout << "(" << c.x << ", " << c.y << ", " << c.z << ")" << endl; cout.flush(); }
template <typename A1>

inline void DEBUG(const A1 &a1) { DEBUG (ToStr(a1)); }
inline std::string ToStr(const char *c) {return string(c); }
inline time_t& ToStr(time_t & t) {return t; }
inline double ToStr(const double &d) { return d; }
inline Uint32 ToStr(const Uint32 &ui) { return ui; }
inline Coord ToStr(const Coord &c) { return c; }
*/
#else

inline void DEBUG_PRINT(...) {}

#endif
#endif /* _DEBUG_ 1 */
