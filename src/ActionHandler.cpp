// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: ActionHandler.cpp,v 1.5 2002-10-20 15:50:27 simon Exp $

#include "ActionHandler.h"

#include <varconf/varconf.h>

#include "common/Log.h"

#include "src/Console.h"
#include "src/ScriptEngine.h"
#include "src/WorldEntity.h"

namespace Sear {

static const bool debug = false;
	
static const std::string SCRIPT = "script";
static const std::string ENTITY = "entity_based";

ActionHandler::ActionHandler(System *system) :
  _system(system),
  _initialised(false)
{}

ActionHandler::~ActionHandler() {
  if (_initialised) shutdown();
}

void ActionHandler::init() {
  if (debug) Log::writeLog("Initialising Action Handler", Log::LOG_DEFAULT);
  if (_initialised) shutdown();
  _initialised = true;
}

void ActionHandler::shutdown() {
  if (debug) Log::writeLog("Shutting down Action Handler", Log::LOG_DEFAULT);
  while(!action_map.empty()) {
    ActionStruct *as = action_map.begin()->second;
    if (as) delete (as);
    action_map.erase(action_map.begin());
  }
  _initialised = false;
}

void ActionHandler::loadConfiguration(const std::string &file_name) {
  varconf::Config config;
  config.sigsv.connect(SigC::slot(*this, &ActionHandler::varconf_callback));
  config.sige.connect(SigC::slot(*this, &ActionHandler::varconf_error_callback));
  config.readFromFile(file_name);
}
  
void ActionHandler::handleAction(const std::string &action, WorldEntity *entity) {
  ActionStruct *as = action_map[action];
  if (as) {
    _system->getScriptEngine()->runScript(as->script);
  }
}

void ActionHandler::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  ActionStruct *record = action_map[section];;
  // If record does not exist, create it.
  if (!record) {
    record = new ActionStruct();
    record->action = section;
    record->entity_based = false;
    action_map[section] = record;
    if (debug) Log::writeLog(std::string("Adding Action: ") + section, Log::LOG_INFO);
  } 
  if (key == SCRIPT) record->script = (std::string)config.getItem(section, key);
  else if (key == ENTITY) record->entity_based = (bool)config.getItem(section, key);
}

void ActionHandler::varconf_error_callback(const char *message) {
  Log::writeLog(message, Log::LOG_ERROR);
}
          

void ActionHandler::registerCommands(Console *console) {
  console->registerCommand(LOAD_CONFIG, this);
  console->registerCommand(DO_ACTION, this);
}
void ActionHandler::runCommand(const std::string &command, const std::string &args) {
  if (command == LOAD_CONFIG) loadConfiguration(args);
  else if (command == DO_ACTION) handleAction(args, NULL);
}

} /* namespace Sear */
