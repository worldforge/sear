// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall

// $Id: FileHandler.cpp,v 1.13 2004-07-19 11:22:14 simon Exp $

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <errno.h>

#include "FileHandler.h"
#include "Console.h"

#ifdef __WIN32__
    #include <io.h> // for _access, Win32 version of stat()
    #include <direct.h> // for _mkdir
#else
    #include <sys/stat.h>
#endif

#ifdef USE_MMGR
  #include "common/mmgr.h"
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
	
FileHandler::FileHandler() {
    std::string installBase = getInstallBasePath();

    addSearchPath(getUserDataPath());
    addSearchPath(".");
    addSearchPath(installBase);
    addSearchPath(installBase + "/scripts");
    
    setVariable("SEAR_INSTALL", installBase);
    setVariable("SEAR_MEDIA", installBase + "/sear-media/");
    setVariable("DEFAULT_SEAR_MEDIA", installBase + "/sear-media/");
    setVariable("SEAR_HOME", getUserDataPath());
    
    if (!exists(getUserDataPath())) {
        std::cout << "creating user data directory at " << getUserDataPath() << std::endl;
#ifdef __WIN32__
        _mkdir(getUserDataPath().c_str()); 
#else
        mkdir(getUserDataPath().c_str(), 0755);
#endif
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
    return std::string(INSTALLDIR) + std::string("/share/sear");
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

void FileHandler::addSearchPath(const std::string &searchpath) {
  _searchpaths.insert(searchpath);
}
void FileHandler::removeSearchPath(const std::string &searchpath) {
  for (FileList::iterator I = _searchpaths.begin(); I != _searchpaths.end(); ++I) {
    std::string path = *I;
    if (path == searchpath) {
      _searchpaths.erase(I);
      return;
    }
  }
}

std::string FileHandler::findFile(const std::string &filename) {
  for (FileList::const_iterator I = _searchpaths.begin(); I != _searchpaths.end(); ++I) {
    std::string filepath = *I + "/" + filename;
    if (exists(filepath))
        return filepath;
  }
  return "";
}

FileHandler::FileList FileHandler::getAllinSearchPaths(const std::string &filename) {
  FileList l;
  for (FileList::const_iterator I = _searchpaths.begin(); I != _searchpaths.end(); ++I) {
    std::string filepath = *I + "/" + filename;
    if (exists(filepath))
      l.insert(filepath);
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

} /* namespace Sear */
