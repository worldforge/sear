// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#ifndef ACTIONHANDLER_H
#define ACTIONHANDLER_H 1

#include "System.h"

#include <map>
#include <string>

#include "ConsoleObject.h"

namespace Sear {

class Console;
class WorldEntity;

class ActionHandler : public ConsoleObject{
public:
  ActionHandler(System *system);
  ~ActionHandler();

  void init();
  void shutdown();

  void loadConfiguration(const std::string &filename);
  
  void handleAction(const std::string &action, WorldEntity *entity);
	
  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);

private:
  
  static const unsigned int MAX_STRING_SIZE = 1024;
  typedef struct {
    char action[MAX_STRING_SIZE];
    char script[MAX_STRING_SIZE];
    bool entity_based;
  } ActionStruct;
  
  std::map<std::string, ActionStruct*> action_map;

  System *_system;
  
  static const char * const LOAD_CONFIG = "load_action_config";
  static const char * const DO_ACTION = "do_action";
};
  
} /* namespace Sear */
#endif /* ACTIONHANDLER_H */
