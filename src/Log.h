// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#ifndef _LOG_H_
#define _LOG_H_ 1

/*
 * This class handles all text i/o
 * Currently only a basic implementation, future
 * work should build on this base and be able to 
 * distinguish between Log types
 */ 

#include <string>

namespace Sear {
	
class Log {
public:
  /*
   * This enumerates the different log entry types
   */
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

  /*
   * This method records an entry into the relevant log location
   */ 
  static void writeLog(const std::string &log_entry, LogLevel type);
  
  // TODO: add ability to redirect each Log type to a different file
  
protected:
};

} /* namespace Sear */

#endif /* _LOG_H_ */
