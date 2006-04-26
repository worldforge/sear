// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: Log.h,v 1.4 2006-04-26 14:38:59 simon Exp $

#ifndef SEAR_LOG_H
#define SEAR_LOG_H 1

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
    LOG_DEFAULT = 0,
    LOG_ERROR,
    LOG_WARNING,
    LOG_ERIS,
    LOG_INFO
  } LogLevel;

  /*
   * This method records an entry into the relevant log location
   */ 
  static void writeLog(const std::string &log_entry, LogLevel type);
  
  // TODO: add ability to redirect each Log type to a different file
  
};

} /* namespace Sear */

#endif /* SEAR_LOG_H */
