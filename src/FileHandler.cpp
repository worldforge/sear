// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall

// $Id: FileHandler.cpp,v 1.7 2004-04-27 15:07:02 simon Exp $

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif
#include <algorithm>

#include <stdio.h>

#include "FileHandler.h"
#include "Console.h"


#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

  static const std::string ADD_SEARCH_PATH = "add_search_path";
  static const std::string REMOVE_SEARCH_PATH = "remove_search_path";
	
FileHandler::FileHandler() {}
FileHandler::~FileHandler() {}

void FileHandler::addSearchPath(const std::string &searchpath) {
  _searchpaths.push_back(searchpath);
}
void FileHandler::removeSearchPath(const std::string &searchpath) {
  for (std::list<std::string>::iterator I = _searchpaths.begin(); I != _searchpaths.end(); ++I) {
    std::string path = *I;
    if (path == searchpath) {
      _searchpaths.erase(I);
      return;
    }
  }
}

std::string FileHandler::findFile(const std::string &filename) {
  for (FileList::const_iterator I = _searchpaths.begin(); I != _searchpaths.end(); ++I) {
    std::string path = *I;
    std::string file = path + "/" + filename;
    FILE *f = fopen(file.c_str(), "r");
    if (f) {
      fclose(f);
      return file;
    }
  }
  return "";
}

FileHandler::FileList FileHandler::getAllinSearchPaths(const std::string &filename) {
  FileList l;
  for (FileList::const_iterator I = _searchpaths.begin(); I != _searchpaths.end(); ++I) {
    std::string path = *I;
    std::string file = path + "/" + filename;
    FILE *f = fopen(file.c_str(), "r");
    if (f) {
      fclose(f);
      l.push_back(file);
    }
  }
  return l;  
}

void FileHandler::registerCommands(Console *console) {
  console->registerCommand(ADD_SEARCH_PATH, this);
  console->registerCommand(REMOVE_SEARCH_PATH, this);
}

void FileHandler::runCommand(const std::string &command, const std::string &args) {
  if (command == ADD_SEARCH_PATH) {
    addSearchPath(args);
  }
  else if (command == REMOVE_SEARCH_PATH) {
    removeSearchPath(args);
  }
}

void FileHandler::expandString(std::string &str) {
  for (VarMap::const_iterator I = varMap.begin(); I != varMap.end(); ++I) {
    std::string var = I->first;
    std::string value = I->second;
    var = "${" + var + "}";
    for (std::string::size_type p=str.find(var); p != str.npos; p=str.find(var, p))
   {
      str.replace(p, var.length(), value);
      p += value.length();
   }

//    replace(str.begin(), str.end(), "${" + var + "}", value);
  }
}

} /* namespace Sear */
