// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Sky.h,v 1.1.2.1 2003-01-05 14:24:41 simon Exp $

#ifndef SEAR_INTERFACE_SKY_H
#define SEAR_INTERFACE_SKY_H 1

namespace Sear {

class Sky {
public:
  Sky() {}
  virtual ~Sky() {}

  virtual void init() = 0;
  virtual void shutdown() = 0;

  virtual void update(float time_elapsed) = 0;

};
	
} /* namespace Sear */

#endif /* SEAR_INTERFACE_SKY_H */
