// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_ 1

#include <iostream.h>
#include <string>

namespace Sear {

class Exception {
public:
  Exception(const std::string &msg) : _msg(msg) {}
  virtual ~Exception() {}

  virtual const std::string getMessage() const { return _msg; }

protected:
  std::string _msg;

};

} /* namespace Sear */

#endif /* _EXCEPTION_H_ */
