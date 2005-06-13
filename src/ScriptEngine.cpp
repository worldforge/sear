// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall

// $Id: ScriptEngine.cpp,v 1.12 2005-06-13 15:10:46 simon Exp $

#include "ScriptEngine.h"

#include <unistd.h>

#include "common/Log.h"

#include "src/Console.h"
#include "src/FileHandler.h"
#include "src/System.h"

#include <fstream>


#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif


#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {
//  static const std::string CHANGE_DIRECTORY = "cd";
//  static const std::string CHANGE_TO_FILE_DIRECTORY = "cd_this_dir";
//  static const std::string ENABLE_DIR_PREFIX = "enable_dir_prefix";
//  static const std::string DISABLE_DIR_PREFIX = "disable_dir_prefix";
  static const std::string RUN_SCRIPT = "run_script";
  static const std::string SEARCH_RUN_SCRIPT = "search_run_script";
 

ScriptEngine::ScriptEngine() :
  _initialised(false),
  _prefix_enabled(true),
  _file_dir("")
{}

ScriptEngine::~ScriptEngine() {
  if (_initialised) shutdown();
}

void ScriptEngine::init() {
  if(_initialised) shutdown();
 // _prefix_enabled = false;	
  _file_dir = "";
  _initialised = true;
}

void ScriptEngine::shutdown() {
  _initialised = false;
}
  
void ScriptEngine::runScript(const std::string &file_name) {
  std::string newFN = file_name;
  System::instance()->getFileHandler()->expandString(newFN);
  std::string string_data;
  if (debug) Log::writeLog(std::string("System: Running script - ") + newFN, Log::LOG_DEFAULT);
//  std::string old_file_dir = _file_dir;
//  std::string::size_type pos = newFN.find_last_of("/");
 // if (pos == std::string::npos) {
 //   pos = newFN.find_last_of("\\");
 // }
 // if (pos == std::string::npos) {
  //  _file_dir = "./";
 // } else {
 //   if (newFN.c_str()[0] == '/' || newFN.c_str()[0] == '\\') {
  //    _file_dir = newFN.substr(0, pos);
   // } else {
    //  _file_dir += "/" + newFN.substr(0, pos); 
    //}
  //}
  std::ifstream script_file(newFN.c_str());
//  char cur_dir[256];
//  memset(cur_dir, '\0', 256);
//  getcwd(cur_dir, 255);
  //if (debug) Log::writeLog(std::string("Current Directory: ") + cur_dir, Log::LOG_DEFAULT);
//  bool pre_cwd = _prefix_enabled; // Store current setting
  if (!script_file) {
    Log::writeLog(std::string("System: Error opening script file: ") + newFN, Log::LOG_ERROR);
    return;
  }
  try {
    while (!script_file.eof()) {
      std::getline(script_file, string_data);
      std::string::size_type pos;
      pos = string_data.find("\r");
      if (pos != std::string::npos) {
        string_data.replace(pos, 1, "");
      }
      pos = string_data.find("#");
      string_data = string_data.substr(0, pos);
//      if(string_data[pos] != '#') 
printf("Exec: %s\n", string_data.c_str());
      System::instance()->getConsole()->runCommand(std::string(string_data));
    }
  } catch (...) {
    fprintf(stderr, "ScriptEngine: Caught Unknown Exception.\n");
    //Arg, caught something, lets clean up before re-throwing the error
  //  _prefix_enabled = pre_cwd;
    //_file_dir = old_file_dir;
    script_file.close();
    throw;
  }
//  chdir(cur_dir); 
  //_prefix_enabled = pre_cwd; // Restore setting
 // _file_dir = old_file_dir;
  script_file.close();
}

void ScriptEngine::registerCommands(Console *console) {
//  console->registerCommand(CHANGE_DIRECTORY, this);
//  console->registerCommand(CHANGE_TO_FILE_DIRECTORY, this);
//  console->registerCommand(ENABLE_DIR_PREFIX, this);
//  console->registerCommand(DISABLE_DIR_PREFIX, this);
  console->registerCommand(RUN_SCRIPT, this);
  console->registerCommand(SEARCH_RUN_SCRIPT, this);
}

void ScriptEngine::runCommand(const std::string &command, const std::string &args) {
//  if (command == CHANGE_DIRECTORY) {
///    if (args.empty()) return;
 //   chdir(args.c_str());
//  }
//  else if (command == ENABLE_DIR_PREFIX) _prefix_enabled = true;
///  else if (command == DISABLE_DIR_PREFIX) _prefix_enabled = false;
//  else 
if (command == RUN_SCRIPT) runScript(args);
//  else if (command == CHANGE_TO_FILE_DIRECTORY) { 
//std::cout << "Changing dir to " << _file_dir << std::endl;
//chdir(_file_dir.c_str());
//}
  else if (command == SEARCH_RUN_SCRIPT) {
    FileHandler::FileList l = System::instance()->getFileHandler()->getAllinSearchPaths(args);
    for (FileHandler::FileList::const_iterator I = l.begin(); I != l.end(); ++I) {
      runScript(*I);
    }
  }
  else  Log::writeLog(std::string("Command not found: - ") + command, Log::LOG_ERROR);
}

} /* namespace Sear */

