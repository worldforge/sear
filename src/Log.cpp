// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#include "Log.h"
#include "iostream.h"

/* LOG AIMS
 * Will provide  ability to log each type to a different file
 * FREQUENT LOG ACTIONS SHOULD BE IN A #if #endif BLOCK and
 * only included in debug mode
 */ 

namespace Sear {
  
void Log::writeLog(const std::string &msg, LogLevel level) {
  std::string type;
  switch (level) {
    case DEFAULT: type = "Default"; break;
    case ERROR: type = "Error"; break;
    case WARNING: type = "Warning"; break;
    case METHOD_ENTRY: type = "Method Entry"; break;
    case METHOD_EXIT: type = "Method Exit"; break;
    case POSITION: type = "Position"; break;
    case ERIS: type = "Eris"; break;		
    case INFO: type = "Info"; break;
  }
  std::cout << "TIMESTAMP - " << type << ": " << msg << std::endl << std::flush;
}

} /* namespace Sear */

