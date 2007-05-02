// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall

// $Id: ActionHandler.cpp,v 1.25 2007-05-02 20:47:55 simon Exp $

#include <unistd.h>

#include <sigc++/object_slot.h>

#include <varconf/varconf.h>

#include "common/Log.h"

#include "src/System.h"
#include "src/Console.h"
#include "src/ScriptEngine.h"
#include "src/WorldEntity.h"

#include "ActionHandler.h"

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
  
namespace Sear {
	
static const std::string LOAD_CONFIG = "load_action_config";
static const std::string DO_ACTION = "do_action";
static const std::string SCRIPT = "script";
static const std::string COMMAND = "command";
static const std::string ENTITY = "entity_based";

ActionHandler::ActionHandler(System *system) :
  m_system(system),
  m_initialised(false)
{
  assert ((system != NULL) && "System is NULL");
}

ActionHandler::~ActionHandler() {
  if (m_initialised) shutdown();
}

void ActionHandler::init() {
  if (debug) Log::writeLog("Action Handler: Initialise", Log::LOG_DEFAULT);

  assert(m_initialised == false);

  m_initialised = true;
}

void ActionHandler::shutdown() {
  assert(m_initialised == true);
  if (debug) Log::writeLog("Action Handler: Shutdown", Log::LOG_DEFAULT);
  // Free all actions
  action_map.clear();

  m_initialised = false;
}

void ActionHandler::loadConfiguration(const std::string &file_name) {
  assert ((m_initialised == true) && "ActionHandler not initialised");
  varconf::Config config;
  // Connect callback to process records
  config.sigsv.connect(sigc::mem_fun(this, &ActionHandler::varconf_callback));
  // Connect callback to catch errors
  config.sige.connect(sigc::mem_fun(this, &ActionHandler::varconf_error_callback));
  // Read the file
  config.readFromFile(file_name);
}
  
void ActionHandler::handleAction(const std::string &action, WorldEntity *entity) {
  assert ((m_initialised == true) && "ActionHandler not initialised");

  // Find requested action
  ActionMap::const_iterator I = action_map.lower_bound(action); 
  ActionMap::const_iterator Iend = action_map.upper_bound(action); 

  if (I == action_map.end()) {
    return;
  }

  for (; I != Iend; ++I) {
    SPtr<ActionStruct> as = I->second;
    // Execute action if it exists
    // Execute command if present, else fall back to external script.
    if (!as->command.empty()) m_system->runCommand(as->command);
    else m_system->getScriptEngine()->runScript(as->script);
  }
}

void ActionHandler::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  // FIXME I am not sure what I have done is right. It is possible differnet
  // varconf records are intended to incrementally set up a single record,
  // so the original code which checked for an existing record, and modified
  // it was probably right.

  // NOTE: Yes this is broken. This code only allows one key to be read in
  //       If there is a second key, then it will create a new astruct.
  //       However, command and script cannot be used together, and the
  //       entity flag is almost alway false. 

  // Create record and set defaults
  SPtr<ActionStruct> record = SPtr<ActionStruct>(new ActionStruct());
  record->action = section;
  record->entity_based = false;

  // Set script file
  if (key == SCRIPT) {
    record->script = (std::string)config.getItem(section, key);
  }
  // Set command string
  else if (key == COMMAND) {
    record->command = (std::string)config.getItem(section, key);
  }
  // Set entity based flag
  else if (key == ENTITY) record->entity_based = (bool)config.getItem(section, key);

  action_map.insert(make_pair(section, record));
  if (debug) Log::writeLog(std::string("Adding Action: ") + section, Log::LOG_INFO);
}

void ActionHandler::varconf_error_callback(const char *message) {
  Log::writeLog(message, Log::LOG_ERROR);
}
          

void ActionHandler::registerCommands(Console *console) {
  assert ((m_initialised == true) && "ActionHandler not initialised");
  assert(console != NULL);
  console->registerCommand(LOAD_CONFIG, this);
  console->registerCommand(DO_ACTION, this);
}

void ActionHandler::runCommand(const std::string &command, const std::string &args) {
  assert ((m_initialised == true) && "ActionHandler not initialised");
  if (command == LOAD_CONFIG) loadConfiguration(args);
  else if (command == DO_ACTION) handleAction(args, NULL);
}

void ActionHandler::addHandler(const std::string &action, const std::string &command) {
  // Get record if it exists
#if 0
  ActionMap::const_iterator I = action_map.find(action);

  if (I != action_map.end()) {
    fprintf(stderr, "Action %s already exists. Ignoring.\n", action.c_str());
    return;
  }
#endif
  
  // Create action 
  SPtr<ActionStruct> record = SPtr<ActionStruct>(new ActionStruct());
  record->action = action;
  record->entity_based = false;
  record->command = command;
  // action_map[action] = record;
  action_map.insert(make_pair(action, record));
}

} /* namespace Sear */
