// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#include "Log.h"

namespace Sear {

Log::Log() : _logfile(cout)
{

}

Log::Log(const std::ostream os) : _logfile(os)
{

}

Log::~Log() {

}

bool Log::init() {
  return true;
}

void Log::shutdown() {

}
  
void Log::changeLogFile(const std::ostream) {

}
  
void Log::writeLog(const std::string &, LogLevel) {

}
  

} /* namespace Sear */

