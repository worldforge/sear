// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#ifndef SEAR_FILEHANDLER_H
#define SEAR_FILEHANDLER_H 1

#include <string>
#include <list>

#include "ConsoleObject.h"

namespace Sear {

class Console;
	
class FileHandler : public ConsoleObject {
public:	
  FileHandler();
  ~FileHandler();

  typedef std::list<std::string> FileList;
  
  void addSearchPath(const std::string &searchpath);
  void removeSearchPath(const std::string &searchpath);

  std::string findFile(const std::string &filename);
  FileList getAllinSearchPaths(const std::string &filename);
  
  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);
  
protected:
  FileList  _searchpaths;

private:
  static const char * const ADD_SEARCH_PATH = "add_search_path";
  static const char * const REMOVE_SEARCH_PATH = "remove_search_path";
  
};

} /* namespace Sear */

#endif /* SEAR_FILEHANDLER_H */
