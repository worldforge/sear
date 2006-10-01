// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: ScriptEngine.cpp,v 1.15 2006-10-01 12:52:44 simon Exp $

#include "ScriptEngine.h"

#include <unistd.h>

#include "common/Log.h"

#include "src/Console.h"
#include "src/FileHandler.h"
#include "src/System.h"

#include <fstream>

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {

static const std::string RUN_SCRIPT = "run_script";
static const std::string SEARCH_RUN_SCRIPT = "search_run_script";

ScriptEngine::ScriptEngine() :
  m_initialised(false),
  m_file_dir("")
{}

ScriptEngine::~ScriptEngine() {
  assert (m_initialised == false);
}

void ScriptEngine::init() {
  assert (m_initialised == false);
  m_file_dir = "";
  m_initialised = true;
}

void ScriptEngine::shutdown() {
  assert (m_initialised == true);
  m_initialised = false;
}
  
void ScriptEngine::runScript(const std::string &file_name) {
  assert (m_initialised == true);
  std::string newFN = file_name;
  System::instance()->getFileHandler()->getFilePath(newFN);
  if (debug) printf("ScriptEngine: Running script: %s\n", newFN.c_str());

  std::ifstream script_file(newFN.c_str());

  if (!script_file) {
    Log::writeLog(std::string("System: Error opening script file: ") + newFN, Log::LOG_ERROR);
    return;
  }
  try {
    std::string string_data;
    while (!script_file.eof()) {
      std::getline(script_file, string_data);
      std::string::size_type pos;
      pos = string_data.find("\r");
      if (pos != std::string::npos) {
        string_data.replace(pos, 1, "");
      }
      pos = string_data.find("#");
      string_data = string_data.substr(0, pos);

      System::instance()->getConsole()->runCommand(std::string(string_data));
    }
  } catch (...) {
    fprintf(stderr, "ScriptEngine: Caught Unknown Exception.\n");
    //Arg, caught something, lets clean up before re-throwing the error
    script_file.close();
    throw;
  }
  script_file.close();
}

void ScriptEngine::registerCommands(Console *console) {
  console->registerCommand(RUN_SCRIPT, this);
  console->registerCommand(SEARCH_RUN_SCRIPT, this);
}

void ScriptEngine::runCommand(const std::string &command, const std::string &args) {
  if (command == RUN_SCRIPT) runScript(args);
  else if (command == SEARCH_RUN_SCRIPT) {
    FileHandler::FileSet l = System::instance()->getFileHandler()->getAllinSearchPaths(args);
    for (FileHandler::FileSet::const_iterator I = l.begin(); I != l.end(); ++I) {
      runScript(*I);
    }
  }
  else  Log::writeLog(std::string("Command not found: - ") + command, Log::LOG_ERROR);
}

} /* namespace Sear */

