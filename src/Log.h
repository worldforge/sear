// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#ifndef _LOG_H_
#define _LOG_H_ 1

#include <iostream.h>
#include <string>

namespace Sear {

typedef enum {
  LOG_DEFAULT = 0,
  LOG_ERRORS
} LogLevel;
	
class Log {
public:
  Log();
  Log(const std::ostream);
  virtual ~Log();

  virtual bool init();
  virtual void shutdown();
  
  virtual void changeLogFile(const std::ostream);
  
  virtual void writeLog(const std::string &, LogLevel);
  
protected:
  std::ostream _logfile;
};

} /* namespace Sear */

#endif /* _LOG_H_ */
