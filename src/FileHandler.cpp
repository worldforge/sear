// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: FileHandler.cpp,v 1.3 2002-09-08 13:08:21 simon Exp $

#include <stdio.h>

#include "FileHandler.h"
#include "Console.h"

namespace Sear {

	
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
