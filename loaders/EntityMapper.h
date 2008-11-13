// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2007 - 2008 Simon Goodall

#ifndef SEAR_LOADERS_ENTITYMAPPER_H
#define SEAR_LOADERS_ENTITYMAPPER_H 1

#include <string>

#include <sigc++/trackable.h>

#include <varconf/config.h>

#include <interfaces/ConsoleObject.h>

namespace Sear {

class Console;
class WorldEntity;

class EntityMapper : public ConsoleObject, public sigc::trackable {
public:
  EntityMapper();
  virtual ~EntityMapper();

  int init();
  void shutdown();
  bool isInitialised() const { return m_initialised; }

  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);

  std::string getEntityMapping(WorldEntity *we);

private:
  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);
  void varconf_error_callback(const char *message);


  typedef std::vector<std::string> StringList;
  typedef std::map<std::string, std::string> StringMap;
  typedef std::map<std::string, StringList> StringListMap;


  bool m_initialised;

  StringMap     m_rules_map;
  StringListMap m_options_map;

};

} /* namespace Sear */

#endif /* SEAR_LOADERS_ENTITYMAPPER_H */
