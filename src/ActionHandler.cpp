// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

// $Id: ActionHandler.cpp,v 1.16 2005-04-13 12:16:04 simon Exp $

#include <unistd.h>
#include <varconf/varconf.h>

#include "common/Log.h"

#include "src/Console.h"
#include "src/ScriptEngine.h"
#include "src/WorldEntity.h"

#include "ActionHandler.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
  
namespace Sear {
	
static const std::string LOAD_CONFIG = "load_action_config";
static const std::string DO_ACTION = "do_action";
static const std::string SCRIPT = "script";
static const std::string ENTITY = "entity_based";

ActionHandler::ActionHandler(System *system) :
  m_system(system),
  m_initialised(false)
{
  assert ((system != NULL) && "System is NULL");
}

ActionHandler::~ActionHandler() {
  assert(m_initialised == false);
}

void ActionHandler::init() {
  if (debug) Log::writeLog("Action Handler: Initialise", Log::LOG_DEFAULT);
  assert(m_initialised == false);

  m_initialised = true;
}

void ActionHandler::shutdown() {
  assert(m_initialised == true);
  if (debug) Log::writeLog("Action Handler: Shutdown", Log::LOG_DEFAULT);
  while(!action_map.empty()) {
    ActionStruct *as = action_map.begin()->second;
    if (as) delete (as);
    action_map.erase(action_map.begin());
  }
  m_initialised = false;
}

void ActionHandler::loadConfiguration(const std::string &file_name) {
  assert ((m_initialised == true) && "ActionHandler not initialised");
  varconf::Config config;
  // Connect callback to process records
  config.sigsv.connect(SigC::slot(*this, &ActionHandler::varconf_callback));
  // Connect callback to catch errors
  config.sige.connect(SigC::slot(*this, &ActionHandler::varconf_error_callback));
  // Read the file
  config.readFromFile(file_name);
}
  
void ActionHandler::handleAction(const std::string &action, WorldEntity *entity) {
  assert ((m_initialised == true) && "ActionHandler not initialised");
  // Get requested action
  ActionStruct *as = action_map[action];
  // Execute action if it exists
  if (as) m_system->getScriptEngine()->runScript(as->script);
}

void ActionHandler::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  // Get record if it exists
  ActionStruct *record = action_map[section];
  // If record does not exist, create it.
  if (!record) {
    // Create record and set defaults
    record = new ActionStruct();
    record->action = section;
    record->entity_based = false;
    action_map[section] = record;
    if (debug) Log::writeLog(std::string("Adding Action: ") + section, Log::LOG_INFO);
  }
  // Set script file
  if (key == SCRIPT) {
    record->script = (std::string)config.getItem(section, key);
  }
  // Set entity based flag
  else if (key == ENTITY) record->entity_based = (bool)config.getItem(section, key);
}

void ActionHandler::varconf_error_callback(const char *message) {
  Log::writeLog(message, Log::LOG_ERROR);
}
          

void ActionHandler::registerCommands(Console *console) {
  assert ((m_initialised == true) && "ActionHandler not initialised");
  console->registerCommand(LOAD_CONFIG, this);
  console->registerCommand(DO_ACTION, this);
}
void ActionHandler::runCommand(const std::string &command, const std::string &args) {
  assert ((m_initialised == true) && "ActionHandler not initialised");
  if (command == LOAD_CONFIG) loadConfiguration(args);
  else if (command == DO_ACTION) handleAction(args, NULL);
}

} /* namespace Sear */
