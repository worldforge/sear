// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

// $Id: FileHandler.h,v 1.10 2005-03-15 17:55:05 simon Exp $

#ifndef SEAR_FILEHANDLER_H
#define SEAR_FILEHANDLER_H 1

#include <string>
#include <list>
#include <set>
#include <map>

#include "interfaces/ConsoleObject.h"


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

  typedef std::set<std::string> FileList;
  
  void addSearchPath(const std::string &searchpath);
  void removeSearchPath(const std::string &searchpath);

  std::string findFile(const std::string &filename);
  FileList getAllinSearchPaths(const std::string &filename);
  
  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);
  
  void setVariable(const std::string &var, const std::string &value) {
    varMap[var] = value;
  }

  std::string getVariable(const std::string &var) {
    return varMap[var];
  }

  void expandString(std::string &str);

    /** Determine if the given file exists at the provided path or not */
    bool exists(const std::string& file) const;

    std::string getInstallBasePath() const;
    
    /** retrive the path of the directory in which to store user setting
    files. This will be in the per-user Application Support folder on Mac and Windows,
    and $HOME/.sear on Unix. If the directory cannot be located, it will
    fall back to '.' */
    std::string getUserDataPath() const;

 /**
  * Make a directory with the given name
  */
  bool mkdir(const std::string &dirname) const;

protected:
  FileList  _searchpaths;

private:
  
  typedef std::map<std::string, std::string> VarMap; 
  VarMap varMap; 
};

} /* namespace Sear */

#endif /* SEAR_FILEHANDLER_H */
