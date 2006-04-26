// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: Log.cpp,v 1.11 2006-04-26 15:26:22 simon Exp $

#include "Log.h"

/* LOG AIMS
 * Will provide  ability to log each type to a different file
 * FREQUENT LOG ACTIONS SHOULD BE IN A #if #endif BLOCK and
 * only included in debug mode
 */ 

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
  
namespace Sear {
  
void Log::writeLog(const std::string &msg, LogLevel level) {
  std::string type;
  switch (level) {
    case LOG_DEFAULT: type = "Default"; break;
    case LOG_ERROR: type = "Error"; break;
    case LOG_WARNING: type = "Warning"; break;
    case LOG_ERIS: type = "Eris"; break;		
    case LOG_INFO: type = "Info"; break;
    default: type="Unknown";
  }
  printf("[%s] %s\n", type.c_str(), msg.c_str());
}

} /* namespace Sear */

