// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: FileHandler.cpp,v 1.25 2007-02-14 11:38:50 simon Exp $

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <errno.h>

#include "FileHandler.h"
#include "Console.h"
#include "System.h"
#include "common/Utility.h"

#include "prefix.h"

#ifdef __WIN32__
    #include <io.h> // for _access, Win32 version of stat()
    #include <direct.h> // for _mkdir
#else
    #include <sys/stat.h>
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

#ifdef __APPLE__

#include <CoreFoundation/CFBundle.h>
#include <CoreServices/CoreServices.h>

std::string getBundleResourceDirPath()
{
    /* the following code looks for the base package directly inside
    the application bundle. This can be changed fairly easily by
    fiddling with the code below. And yes, I know it's ugly and verbose.
    */
    CFBundleRef appBundle = CFBundleGetMainBundle();
    CFURLRef resUrl = CFBundleCopyResourcesDirectoryURL(appBundle);
    CFURLRef absResUrl = CFURLCopyAbsoluteURL(resUrl);
   
    // now convert down to a path, and the a c-string
    CFStringRef path = CFURLCopyFileSystemPath(absResUrl, kCFURLPOSIXPathStyle);
    std::string result = CFStringGetCStringPtr(path, CFStringGetSystemEncoding());

    CFRelease(resUrl);
    CFRelease(absResUrl);
    CFRelease(path);        
    return result;
}

std::string getAppSupportDirPath()
{
    FSRef fs;
    OSErr err = FSFindFolder(kUserDomain, kApplicationSupportFolderType, true, &fs);
    if (err != noErr) {
        std::cerr << "error doing FindFolder" << std::endl;
        return std::string();
    }

    CFURLRef dirURL = CFURLCreateFromFSRef(kCFAllocatorSystemDefault, &fs);
    char fsRepr[1024];
    if (!CFURLGetFileSystemRepresentation(dirURL, true, (UInt8*) fsRepr, 1024)) {
        std::cerr << "error invoking CFURLGetFileSystemRepresentation" << std::endl;
        return std::string();
    }

    CFRelease(dirURL);
    return fsRepr;
}

#endif

namespace Sear {

  static const std::string ADD_SEARCH_PATH = "add_search_path";
  static const std::string REMOVE_SEARCH_PATH = "remove_search_path";

  static const std::string INSERT_FILE_PATH = "insert_file_path";
  static const std::string APPEND_FILE_PATH = "append_file_path";
  static const std::string REMOVE_FILE_PATH = "remove_file_path";
  static const std::string CLEAR_FILE_PATH = "clear_file_path";
  static const std::string GET_FILE_PATH = "get_file_path";
  // Older var commands
  static const std::string CMD_GETVAR = "getvar";
  static const std::string CMD_SETVAR = "setvar";
  // newer ones
  static const std::string CMD_GET_VARIABLE = "get_variable";
  static const std::string CMD_SET_VARIABLE = "set_variable";
  static const std::string CMD_DELETE_VARIABLE = "delete_variable";
	
FileHandler::FileHandler() {
    std::string installBase = getInstallBasePath();

    addSearchPath(getUserDataPath());
    addSearchPath(".");
    addSearchPath(installBase);
    addSearchPath(installBase + "/scripts");
    
    // This is the prefix 
    setVariable("SEAR_INSTALL", installBase);
    // This is the user's home dir
    setVariable("SEAR_HOME", getUserDataPath());
    // Search $HOME/.sear for media first
    insertFilePath("SEAR_MEDIA", "${SEAR_HOME}/sear-media-0.6/");
    // Check prefix location second
    appendFilePath("SEAR_MEDIA", "${SEAR_INSTALL}/sear-media-0.6/");
    
    if (!exists(getUserDataPath())) {
      std::cout << "creating user data directory at " << getUserDataPath() << std::endl;
      mkdir(getUserDataPath());
    }
}

FileHandler::~FileHandler() {}

std::string FileHandler::getInstallBasePath() const
{
#ifdef __APPLE__
  return getBundleResourceDirPath();
#elif __WIN32__
  return std::string(".");
#else
  #ifdef ENABLE_BINRELOC
  return std::string(DATADIR) + std::string("/sear");
#else
  return std::string(INSTALLDIR) + std::string("/share/sear");
#endif
#endif
}

std::string FileHandler::getUserDataPath() const
{
#ifdef __WIN32__
  std::string path = getenv("USERPROFILE");
  if (path.empty()) {
    const char *homedrive = getenv("HOMEDRIVE");
    const char *homepath = getenv("HOMEPATH");
    
    if (!homedrive || !homepath) {
        std::cerr << "unable to determine homedir in Win32, using ." << std::endl;
        return ".";
    }
    path = std::string(homedrive) + std::string(homepath);
  }
  return path + "\\Application Data\\Sear\\";
#elif __APPLE__
    return getAppSupportDirPath() + "/Sear/";
#else
  std::string path = getenv("HOME");
  if (path.empty()) {
    std::cerr << "$HOME not set, using '.' for user settings" << std::endl;
    return ".";
  }
  
  return path + "/.sear/";
#endif
}


void FileHandler::insertFilePath(const std::string &var, const std::string &path) {
  m_file_map[var].push_front(path);
}

void FileHandler::appendFilePath(const std::string &var, const std::string &path) {
  m_file_map[var].push_back(path);
}

void FileHandler::removeFilePath(const std::string &var, const std::string &path) {
  // TODO: implement
  fprintf(stderr, "FileHandler::removeFilePath is not implemented yet.\n");
}

void FileHandler::clearFilePath(const std::string &var) {
  StringListMap::iterator I = m_file_map.find(var);
  if (I != m_file_map.end()) m_file_map.erase(I);
}

FileHandler::FileList FileHandler::getFilePaths(const std::string &str) {
  FileList fl;
  // First pass at string expansion
  std::string cpy = str;
  expandString(cpy);
  // No point doing any further expansion if the string is already a real path.
  if (exists(cpy)) {
    fl.push_back(cpy);
    return fl;
  }

  // Next we loop through each file path var and each path until we find
  // one that is a real file. Otherwise we just return the expanded string.
  StringListMap::const_iterator I = m_file_map.begin();
  StringListMap::const_iterator Iend = m_file_map.end();
  while (I != Iend) {
    const std::string &key = "${" + I->first + "}";
    std::string::size_type pos = cpy.find(key);
    if (pos != std::string::npos) {
      const StringList &l = I->second;
      StringList::const_iterator J = l.begin();
      StringList::const_iterator Jend = l.end();
      while (J != Jend) {
        std::string cpy2 = cpy;
        const std::string &value = *J;
        cpy2.replace(pos, key.length(), value);
        // Expand any new variables that may have been passed in.
        expandString(cpy2);
        // If we have found a file, then we are finished here
        if (exists(cpy2)) {
          fl.push_back(cpy2);
          break;
        }
        ++J;
      }
    }
    ++I;
  }
  return fl;
}

void FileHandler::getFilePath(std::string &cpy) {
  const FileList &l = getFilePaths(cpy);

  if (l.size() > 0) {
    cpy = *l.begin();
  }
}



void FileHandler::addSearchPath(const std::string &searchpath) {
  m_searchpaths.insert(searchpath);
}
void FileHandler::removeSearchPath(const std::string &searchpath) {
  FileSet::iterator I = m_searchpaths.find(searchpath);
  if (I != m_searchpaths.end()) {
    m_searchpaths.erase(I);
  }
}

std::string FileHandler::findFile(const std::string &filename) {
  FileSet::const_iterator I = m_searchpaths.begin();
  FileSet::const_iterator Iend = m_searchpaths.end();
  for (;I != Iend; ++I) {
    const std::string &filepath = *I + "/" + filename;
    if (exists(filepath))
        return filepath;
  }
  return "";
}

FileHandler::FileSet FileHandler::getAllinSearchPaths(const std::string &filename) {
  FileSet l;
  FileSet::const_iterator I = m_searchpaths.begin();
  FileSet::const_iterator Iend = m_searchpaths.end();
  for (;I != Iend; ++I) {
    const std::string &filepath = *I + "/" + filename;
    if (exists(filepath))
      l.insert(filepath);
  }
  return l;  
}

void FileHandler::registerCommands(Console *console) {
  console->registerCommand(ADD_SEARCH_PATH, this);
  console->registerCommand(REMOVE_SEARCH_PATH, this);
  console->registerCommand(CMD_SETVAR, this);
  console->registerCommand(CMD_GETVAR, this);
  console->registerCommand(CMD_SET_VARIABLE, this);
  console->registerCommand(CMD_GET_VARIABLE, this);
  console->registerCommand(CMD_DELETE_VARIABLE, this);
  console->registerCommand(INSERT_FILE_PATH, this);
  console->registerCommand(APPEND_FILE_PATH, this);
  console->registerCommand(REMOVE_FILE_PATH, this);
  console->registerCommand(CLEAR_FILE_PATH, this);
  console->registerCommand(GET_FILE_PATH, this);
}

void FileHandler::runCommand(const std::string &command, const std::string &args) {
  Tokeniser tokeniser;
  tokeniser.initTokens(args);
  if (command == ADD_SEARCH_PATH) {
    addSearchPath(args);
  }
  else if (command == REMOVE_SEARCH_PATH) {
    removeSearchPath(args);
  }
  else if (command == CMD_SETVAR || command == CMD_SET_VARIABLE) {
    const std::string &key = tokeniser.nextToken();
    const std::string &value = tokeniser.remainingTokens();
    setVariable(key, value);
  }
  else if (command == CMD_GETVAR || command == CMD_GET_VARIABLE) {
    std::string key = tokeniser.nextToken();
    System::instance()->pushMessage(getVariable(key), CONSOLE_MESSAGE);    
  }
  else if (command == CMD_DELETE_VARIABLE) {
    deleteVariable(args);
  }
  else if (command == INSERT_FILE_PATH) {
    const std::string &var = tokeniser.nextToken();
    const std::string &path = tokeniser.remainingTokens();
    insertFilePath(var, path);
  }
  else if (command == APPEND_FILE_PATH) {
    const std::string &var = tokeniser.nextToken();
    const std::string &path = tokeniser.remainingTokens();
    appendFilePath(var, path);
  }
  else if (command == REMOVE_FILE_PATH) {
    const std::string &var = tokeniser.nextToken();
    const std::string &path = tokeniser.remainingTokens();
    removeFilePath(var, path);
  }
  else if (command == CLEAR_FILE_PATH) {
    clearFilePath(args);
  }
   else if (command == GET_FILE_PATH) {    
    std::string f = args;
    getFilePath(f);
    System::instance()->pushMessage(f, CONSOLE_MESSAGE);    
  }
}

void FileHandler::expandString(std::string &str) {
  bool changed = true;
  while (changed) {
    changed = false;
    VarMap::const_iterator I = m_varMap.begin();
    VarMap::const_iterator Iend = m_varMap.end();
    for (; I != Iend; ++I) {
      const std::string &var = I->first;
      const std::string &value = I->second;
      const std::string &new_var = "${" + var + "}";
      for (std::string::size_type p=str.find(new_var); p != str.npos; p=str.find(new_var, p))
     {
        str.replace(p, new_var.length(), value);
        p += value.length();
        changed = true;
     }
    }
  }
}

bool FileHandler::exists(const std::string& file) const
{
#ifdef __WIN32__
    int ret = _access(file.c_str(), 0x04); // read access
    return (ret == 0);
#else
    struct stat info;
    int ret = ::stat(file.c_str(), &info);
    
    if (ret==0) return true;
    // this error is fine
    if (errno == ENOENT) return false;

    char msgBuf[1024];
    ::strerror_r(errno, msgBuf, 1024);
    std::cerr << "got error " << msgBuf << " doing stat() of " << file << std::endl;
    
    return false;
#endif
}

bool FileHandler::mkdir(const std::string &dirname) const {
int err = 0;
#ifdef __WIN32__
       err =  _mkdir(dirname.c_str()); 
#else
       err =  ::mkdir(dirname.c_str(), 0755);
#endif
  return (err == 0);

}

} /* namespace Sear */
