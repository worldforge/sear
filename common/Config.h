// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _CONFIGURE_H_
#define _CONFIGURE_H_ 1

#include <map>
#include <string>

namespace Sear {

class Config {
public:
  Config() {}
  ~Config() {}
  
  bool init() {
    _attributes = std::map<std::string, std::string>();
    return true;
  }
  
  void shutdown();
  
  void loadConfig(const std::string &file_name, bool prefix_cwd = false);
  
  void saveConfig();
  void saveConfig(const std::string &file_name);
  
  std::string getAttribute(const std::string &);
  void setAttribute(const std::string &, const std::string &);
  std::map<std::string, std::string> getMap() { return _attributes; }

protected:
  static const char * const  READ_ACCESS = "r";
  static const char * const WRITE_ACCESS = "w";
  static const char * const FORMAT = "%s = %[^\n]";
  static const int MAX_SIZE = 512;

  std::string _last_file_name;
  std::map<std::string, std::string> _attributes;
};

} /* namespace Sear */
#endif /* _CONFIGURE_H_ */

