// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#ifndef _LOG_H_
#define _LOG_H_ 1

#include <string>

namespace Sear {

	
class Log {
public:
typedef enum {
  DEFAULT = 0,
  ERROR,
  WARNING,
  METHOD_ENTRY,
  METHOD_EXIT,
  POSITION,
  ERIS,
  INFO
} LogLevel;

  static void writeLog(const std::string &, LogLevel);
  
protected:
};

} /* namespace Sear */

#endif /* _LOG_H_ */
