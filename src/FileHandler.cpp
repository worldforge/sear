// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: FileHandler.cpp,v 1.5 2003-03-06 23:50:38 simon Exp $

#include <stdio.h>

#include "FileHandler.h"
#include "Console.h"

#ifdef HAVE_CONFIG
  #include "config.h"
#endif

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
} /* namespace Sear */
