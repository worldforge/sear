// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#ifndef _SHAPE_H_
#define _SHAPE_H_ 1

namespace Sear {

class Shape {
public:
  Shape() {}
  virtual ~Shape() {}

  virtual bool init() { return false; }
  virtual void shutdown() {}


};

}

#endif /* _SHAPE_H_ */
