// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#include "ActionHandler.h"
#include "Console.h"
#include "WorldEntity.h"

namespace Sear {
	
ActionHandler::ActionHandler(System *system) :
  _system(system)
{

}

ActionHandler::~ActionHandler() {

}

void ActionHandler::init() {

}

void ActionHandler::shutdown() {
  while(!action_map.empty()) {
    ActionStruct *as = action_map.begin()->second;
    if (as) free (as);
    action_map.erase(action_map.begin());
  }
}

void ActionHandler::loadConfiguration(const std::string &filename) {
  FILE *file = NULL;
  file = fopen(filename.c_str(),"r");
  if (!file) return;
  while (!feof(file)) {
    ActionStruct *as = (ActionStruct*)malloc(sizeof(ActionStruct));
    char *str = NULL, *str_start;
    char string_data[MAX_STRING_SIZE];
    str = (char*)malloc(MAX_STRING_SIZE);
    memset(string_data, '\0', MAX_STRING_SIZE);
    memset(str, '\0', MAX_STRING_SIZE);
    memset(as, 0, sizeof(ActionStruct));
    str_start = str;
    // Get object type name
    while (!feof(file)) {
      str = str_start;
      memset(string_data, '\0', MAX_STRING_SIZE);
      memset(str, '\0', MAX_STRING_SIZE);
      fgets(str, MAX_STRING_SIZE, file);
      sscanf(str, "%s\n", &string_data[0]);
      std::string tag = std::string(string_data);
      if (tag == "end_of_action") {
        // If no name was found, return NULL;         
        if (!as->action) {
          free(as);
          as = NULL;
        }
        if (str) free(str);
	break;
      }
      sscanf(str, "%s = %*s\n", &string_data[0]);
      tag = std::string(string_data);
      if (tag == "action") {
        sscanf(str, "%*s = %s\n", &as->action[0]);
      }
      else if (tag == "script") {
        sscanf(str, "%*s = %s\n", &as->script[0]);
      }
      if (tag == "entity_based") {
        sscanf(str, "%*s = %s\n", &string_data[0]);
        as->entity_based = (std::string(string_data) == "true") ? (true) : (false);
      }
    }
    if (as) {
      action_map[as->action] = as;
    }
  }
}
  
void ActionHandler::handleAction(const std::string &action, WorldEntity *entity) {
  ActionStruct *as = action_map[action];
  if (as) {
    _system->runScript(as->script);
  }
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
