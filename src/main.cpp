#include <iostream>

#include "engine/System.h"
#include "script/ScriptEngine.h"

int main (int argc, char **argv) {
  std::cout << "Sear" << std::endl;
  Sear::System *system = new Sear::System();
  system->init();
  system->getScriptEngine()->runFile("scripts/startup.lua");
//  system->getScriptEngine()->runFile("test.lua");
  system->mainLoop();
  system->shutdown();
  delete system;
  return 0;
}
