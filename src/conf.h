// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#define CLIENT_NAME "Sear"
#define DEFAULT_PORT 6767

#ifdef M_PI
#define SEAR_PI M_PI
#else
#define SEAR_PI (3.14159265358979323846)
#endif


#define DEG_TO_RAD (SEAR_PI / 180.0f)
#define RAD_TO_DEG (180.0f / SEAR_PI)

#include "Error.h"
