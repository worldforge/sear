// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: FileHandler.h,v 1.4 2003-03-06 23:50:38 simon Exp $

#ifndef SEAR_FILEHANDLER_H
#define SEAR_FILEHANDLER_H 1

#include <string>
#include <list>

#include "ConsoleObject.h"


/*
 * Currently returns file names from a list of search paths
 * TODO
 * This class will be used for all file handling issues.
 * Aim is to link in a virtual file system for easy media packaging
 * this could be from a zip file, or a custom format.
 * It would also be good to be able to use file-memory mapping if possible/appropriate.
 */ 

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
  
};

} /* namespace Sear */

#endif /* SEAR_FILEHANDLER_H */
